/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <new>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "audio/impl/alsa_reader.h"

#define APP_NUM_PERIODS	256U
#define DRIVER_PERIODS	3U

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class ALSASource
///////////////////////////////////////////////////////////////////////////////
/// \brief The ALSASource class
///

class ALSASource :
        public AAPIObject
{
    DECLARE_AAPI_OBJECT(ALSASource)

public:
    snd_pcm_t           *handle;

    /* from cmd line parameters */
    snd_pcm_format_t    format;
    uint32_t            channels;
    uint32_t            rate;
    uint32_t            period_bytes;  /* bytes in a period */

    /* derived from cmd line params */
    snd_pcm_uframes_t   period_frames; /* frames in a period */
    uint32_t            bits_per_sample;
    uint32_t            bits_per_frame;

    /* from sound hardware */
    snd_pcm_uframes_t   buffer_frames;  /* size of buffer in frames */

    /* buffer data */
    char                *buffer;
    int                 read_position; /* in bytes */
    int                 clnt_position;

    int                 read_periods;
    int                 clnt_periods;

    /* locking/threading */
    pthread_t           rtid, ctid;
    pthread_mutex_t     mutex; /* for access to reader/writer pos */
    sem_t               sem; /* for waking writer */

    /* complete flag*/
    volatile uint8_t    complete;

    /* consumer callback */
    AAPIAudioReaderEvents *callback;

    ALSASource()
        : handle(nullptr)
        , rtid(0)
        , ctid(0)
        , callback(nullptr)
    { }

    ~ALSASource()
    {
        close(this);
    }

    static int open(ALSASource *source, const char *device, uint32_t channels,
                    uint32_t sample_rate, uint32_t sample_size, uint32_t num_samples)
    {
        if( source->handle != nullptr )
        {
            return AAPIAUDIO_E_ALREADY_OPEN;
        }

        /* Period size in bytes. */
        uint32_t period_size;
        int ret;

        /* configure */
        switch( sample_size )
        {
        case 16:
            source->format = SND_PCM_FORMAT_S16_LE;
            period_size = num_samples * 2 /*sample size = 2 bytes*/;
            break;
        case 24:
            source->format = SND_PCM_FORMAT_S24_LE;
            period_size = num_samples * 4 /*sample size = 4 bytes*/;
            break;
        case 32:
            source->format = SND_PCM_FORMAT_S32_LE;
            period_size = num_samples * 4 /*sample size = 4 bytes*/;
            break;
        default:
            return AAPIAUDIO_E_INVALID_PARAM;
        }

        source->channels = channels;
        source->rate = sample_rate;
        source->period_bytes = period_size * channels;

        /* for debug */
        ret = snd_output_stdio_attach( &log, stderr, 0 );

        /* open the alsa source */
        ret = snd_pcm_open( &source->handle, device,
                            SND_PCM_STREAM_CAPTURE, 0);
        if( ret < 0 )
        {
            /* audio open error */;
            return AAPIAUDIO_E_OPEN_DEVICE;
        }

        /* configure audio */
        ret = set_params( source );
        if( ret < 0 )
        {
            /* set params error */
            snd_pcm_close( source->handle );
            source->handle = nullptr;
            return AAPIAUDIO_E_INVALID_PARAM;
        }

        source->buffer = reinterpret_cast<char *>(
                    malloc( source->period_bytes * APP_NUM_PERIODS ));
        if( !source->buffer )
        {
            /* Unable to allocate a reader buffer. */
            snd_pcm_close( source->handle );
            source->handle = nullptr;
            return AAPI_E_OUT_OF_MEMORY;
        }

        /* init mutex */
        pthread_mutex_init( &source->mutex, nullptr );

        return 0;
    }

    static void close(ALSASource *alsa)
    {
        if( !alsa->handle )
        {
            return;
        }

        stop( alsa );

        /* cleanup */
        snd_pcm_nonblock( alsa->handle, 0 );
        snd_pcm_drain( alsa->handle );
        snd_pcm_close( alsa->handle );

        /* Reset the handle */
        alsa->handle = nullptr;

        /* Release buffer */
        free( alsa->buffer );

        /* destroy mutex */
        pthread_mutex_destroy( &alsa->mutex );

        // TODO: think about this
        //snd_output_close(alsa_data->log);
    }

    static int start(ALSASource *source, AAPIAudioReaderEvents* callback)
    {
        if( source->rtid != 0 )
        {
            return 0;
        }

        /* Reset flags. */
        source->complete = 0;

        /* Set callback */
        source->callback = callback;

        /* Reset all positions/counters */
        source->read_position = 0;
        source->clnt_position = 0;
        source->read_periods = 0;
        source->clnt_periods = 0;

        /* init semaphore */
        sem_init( &source->sem, 0, 0 );

        /* init threads */
        int ret = pthread_create( &source->rtid, nullptr,
                                 read_thread, source );
        if( ret < 0 )
        {
            /* failed to create thread */
            return AAPI_E_CREATE_THREAD;
        }

        /* Sleep 10msec*/
        usleep( 10000 );

        ret = pthread_create( &source->ctid, nullptr,
                             client_thread, source );
        if( ret < 0 )
        {
            pthread_detach( source->rtid );
            source->rtid = 0;
            /* failed to create thread */
            return AAPI_E_CREATE_THREAD;
        }

        return 0;
    }

    static void stop(ALSASource *alsa_data)
    {
        if( alsa_data->rtid == 0 )
        {
            return;
        }

        void *rend,*cend;

        /* Set complete flag. */
        alsa_data->complete = 1;

        /* Wait for workers to finish */
        pthread_join( alsa_data->rtid, &rend );
        pthread_join( alsa_data->ctid, &cend );

        alsa_data->rtid = 0;
        alsa_data->ctid = 0;

        /* Destroy semaphore */
        sem_destroy( &alsa_data->sem );
    }

    static void *read_thread(void *data)
    {
        ALSASource *alsa_data = reinterpret_cast< ALSASource *>( data );
        sched_param sparam;
        int policy;

        /* Alter thread priority */
        pthread_getschedparam( pthread_self(), &policy, &sparam );
        sparam.sched_priority = sched_get_priority_max( policy );
        pthread_setschedparam( pthread_self(), policy, &sparam );

        /* Exit the loop after complete flag is set */
        while( !alsa_data->complete )
        {
            pcm_read( alsa_data, alsa_data->period_bytes );
        }

        return nullptr;
    }

    static void *client_thread(void *data)
    {
        ALSASource *source = reinterpret_cast<ALSASource *>( data );
        uint32_t channel_bytes = source->period_bytes / source->channels;
        uint32_t bytes_per_frame = source->bits_per_frame / 8;
        uint32_t bytes_per_sample = source->bits_per_sample / 8;
        char **channel_samples =
                reinterpret_cast<char **>(alloca( sizeof(char*) * source->channels ));
        char *buffer = source->buffer + source->clnt_position;
        sched_param sparam;
        int policy, val;
        uint32_t bytes, i;

        /* allocate buffer for every channel */
        for( i = 0; i < source->channels; i++ )
        {
            channel_samples[ i ] =
                    reinterpret_cast<char *>( alloca( channel_bytes ) );
        }

        /* alter thread priority */
        pthread_getschedparam( pthread_self(), &policy, &sparam );
        sparam.sched_priority = sched_get_priority_max( policy );
        pthread_setschedparam( pthread_self(), policy, &sparam );

        while( true )
        {
            /* Wait for next frame to be avail */
            sem_wait( &source->sem );

            /* Pointer wrap ?*/
            if( source->clnt_position == source->period_bytes * APP_NUM_PERIODS )
            {
                source->clnt_position = 0;
                buffer = source->buffer;
            }

            /* Consume whole period */
            bytes = source->period_bytes;

            if( source->callback )
            {
                /* Split channels and notify separated buffers */
                for( i = 0; i < source->channels; i++ )
                {
                    char *frame_data = buffer;
                    char *sample_data = channel_samples[i];

                    for( uint f = 0; f < source->period_frames; f++ )
                    {
                        /* Copy single channel sample data */
                        memcpy( sample_data, frame_data + i*bytes_per_sample, bytes_per_sample );
                        frame_data += bytes_per_frame;
                        sample_data += bytes_per_sample;
                    }
                }

                source->callback->audio_reader_data( channel_samples, source->channels, channel_bytes );
            }

            source->clnt_periods++;

            /* inc consumer pos */
            source->clnt_position += bytes;

            /* inc buffer pointer */
            buffer = source->buffer + source->clnt_position;

            /* check how many periods are waiting to be consumed */
            sem_getvalue( &source->sem, &val );

            /* has consumer been overtaken by the reader ?*/
            if( val >= APP_NUM_PERIODS )
            {
                // overwrite
            }
            if( val < APP_NUM_PERIODS )
            {
            }

            if( val == 0 && source->complete )
            {
                break;
            }
        }

        /* Writer finished */
        return nullptr;
    }

    /* sets the audio parameters */
    static int set_params(ALSASource *alsa_data)
    {
        snd_pcm_hw_params_t *params;
        snd_pcm_sw_params_t *swparams;
        int ret;

        snd_pcm_hw_params_alloca( &params );
        snd_pcm_sw_params_alloca( &swparams );

        /* number of bits per sample e.g. 16 for S16_LE */
        alsa_data->bits_per_sample = static_cast< uint32_t >(
                snd_pcm_format_physical_width( alsa_data->format ));

        /* number of bits in alsa frame e.g. 32 for S16_LE stereo */
        alsa_data->bits_per_frame =
                alsa_data->bits_per_sample * alsa_data->channels;

        /* number of frames in a period (period bytes / frame bytes) */
        alsa_data->period_frames =
                alsa_data->period_bytes / (alsa_data->bits_per_frame >> 3);

        /* config Hardware params */
        ret = snd_pcm_hw_params_any( alsa_data->handle, params );
        if( ret < 0 )
        {
            /* Broken configuration for this PCM */
            return -ENODEV;
        }

        /* set pcm format to be interleaved (e.g. LRLRLR for stereo) */
        ret = snd_pcm_hw_params_set_access( alsa_data->handle,
                            params, SND_PCM_ACCESS_RW_INTERLEAVED );
        if( ret < 0 )
        {
            /* Access type not available */
            return -EINVAL;
        }

        /* sample format e.g. 16 bit little endian S16_LE */
        ret = snd_pcm_hw_params_set_format( alsa_data->handle,
                            params, alsa_data->format );
        if( ret < 0 )
        {
            /* Sample format non available */
            return -EINVAL;
        }

        /* number of channels */
        ret = snd_pcm_hw_params_set_channels( alsa_data->handle,
                            params, alsa_data->channels );
        if( ret < 0 )
        {
            /* Channels count non available */
            return -EINVAL;
        }

        /* rate (or nearest) */
        ret = snd_pcm_hw_params_set_rate_near( alsa_data->handle,
                            params, &alsa_data->rate, 0 );
        if( ret < 0 )
        {
            /* rate non available */
            return -EINVAL;
        }

        /* config hardware buffering */

        /* get max supported buffer size */
        ret = snd_pcm_hw_params_get_buffer_size_max(
                            params, &alsa_data->buffer_frames );

        /* we want buffer to be atleast DRIVER_PERIODS in size */
        if( (alsa_data->buffer_frames / alsa_data->period_frames) < DRIVER_PERIODS )
        {
            /* ALSA buffer too small */
            return -EINVAL;
        }

        /* set required period size */
        ret = snd_pcm_hw_params_set_period_size( alsa_data->handle,
                            params, alsa_data->period_frames, 0 );
        if( ret < 0 )
        {
            /* period size not available */
            return -EINVAL;
        }

        /* set required buffer size (or nearest)*/
        ret = snd_pcm_hw_params_set_buffer_size_near( alsa_data->handle,
                            params, &alsa_data->buffer_frames );
        if( ret < 0 )
        {
            /* buffer size not available */
            return -EINVAL;
        }

        /* commit all above hardware audio parameters to driver */
        ret = snd_pcm_hw_params( alsa_data->handle, params );
        if( ret < 0 )
        {
            /* Unable to install hw params */
            return -EINVAL;
        }

        /* config software audio params */
        snd_pcm_sw_params_current( alsa_data->handle, swparams );

        ret = snd_pcm_sw_params_set_avail_min( alsa_data->handle,
                            swparams, alsa_data->period_frames );
        if( ret < 0 )
        {
            /* failed to set avail min */
            return -EINVAL;
        }

        /* frames for alsa-lib/driver to buffer internally before starting */
        ret = snd_pcm_sw_params_set_start_threshold( alsa_data->handle,
                            swparams, 1 );
        if( ret < 0 )
        {
            /* failed to set start threshold */
            return -EINVAL;
        }

        /* if free frames >= buffer frames then stop */
        ret = snd_pcm_sw_params_set_stop_threshold( alsa_data->handle,
                            swparams, alsa_data->buffer_frames );
        if( ret < 0 )
        {
            /* failed to set stop threshold */
            return -EINVAL;
        }

        /* commit the software params to alsa-lib */
        ret = snd_pcm_sw_params( alsa_data->handle, swparams );
        if( ret < 0 )
        {
            /* unable to install sw params */
            snd_pcm_sw_params_dump( swparams, log );
            return -EINVAL;
        }

        snd_pcm_dump(alsa_data->handle, log);
        return 0;
    }

    static void xrun(ALSASource *alsa)
    {
        /* Handle device overrun */
        snd_pcm_recover(alsa->handle, -EPIPE, 0);
    }

    /* read pcm data from the audio driver */
    static ssize_t pcm_read(ALSASource *alsa, size_t count)
    {
        char *data = alsa->buffer + alsa->read_position;
        ssize_t size;
        ssize_t result = 0;
        snd_pcm_uframes_t frames;
        int wait, val, overwrite = 0;

        /* check how many periods are free for reader */
        sem_getvalue(&alsa->sem, &val);

        /* has we overtaken the writer ?? */
        if (val >= APP_NUM_PERIODS - 1)
        {
            /*
             * We have many choices here (depending on our policy):
             * 1. Restart capture.
             * 2. Discard current period and wait for writer to catch up.
             * 3. Discard/flush writer periods.
             * We choose option 2.
             */
            /* overwrite */
            overwrite = 1;
        }

        /* change count from bytes to frames */
        frames = count / (alsa->bits_per_frame >> 3);

        /* read a number of frames from the driver */
        while (frames > 0)
        {
            size = snd_pcm_readi(alsa->handle, data, frames);

            if (size == -EAGAIN || (size >= 0 && size < frames))
            {
                /* wait 1000ms max */
                wait = snd_pcm_wait( alsa->handle, 1000 );
                if (wait <= 0)
                {
                    /* wait timeout/error */
                }
            }
            else if (size == -EPIPE)
            {
                /* device overrun */
                xrun(alsa);
            }
            else if (size == -ESTRPIPE)
            {
                /* suspend(); */
            }
            else if (size < 0)
            {
                /* read error */
            }

            /* still have data to read ? */
            if (size > 0)
            {
                result += size;
                frames -= size;
                data += size * alsa->bits_per_frame / 8;
            }
        }

        /* return if we are about to overwrrite */
        if (overwrite)
            return result;

        /* got frames, so update reader pointer */
        alsa->read_position += count;

        /* buffer wrap ? */
        if (alsa->read_position >= alsa->period_bytes * APP_NUM_PERIODS)
        {
            alsa->read_position = 0;
        }

        /* tel writer we have frame */
        sem_post(&alsa->sem);
        alsa->read_periods++;

        return result;
    }

    /* for debug logging */
    static snd_output_t *log;
};

snd_output_t *ALSASource::log = nullptr;

///////////////////////////////////////////////////////////////////////////////
// class ALSADevice
///////////////////////////////////////////////////////////////////////////////

class ALSADevice
{
public:
    ALSADevice()
        : card_id(nullptr)
        , pcm_id(nullptr)
        , card_name(nullptr)
        , pcm_name(nullptr)
    { }

    ~ALSADevice()
    {
        free(card_id);
        free(pcm_id);
        free(card_name);
        free(pcm_name);
    }

    int card_i;
    int pcm_i;
    char *card_id;
    char *pcm_id;
    char *card_name;
    char *pcm_name;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIALSAReader
///////////////////////////////////////////////////////////////////////////////

AAPIALSAReader::AAPIALSAReader()
    : m_source(nullptr)
    , m_devices(nullptr)
    , m_numDevices(0)
{
    load_devices( &m_devices, &m_numDevices );
}

AAPIALSAReader::~AAPIALSAReader()
{
    free( m_devices );
}

#define ALLOC_SLOTS  5

void AAPIALSAReader::load_devices(ALSADevice **devices, uint32_t *num_devices)
{
    ALSADevice *buffer;
    ALSADevice *device;
    int count = 0;
    size_t alloc_size = ALLOC_SLOTS;
    char card_id[10]={0};
    char pcm_id[10]={0};

    buffer = reinterpret_cast< ALSADevice *> (
                malloc (sizeof (ALSADevice) * alloc_size));

    int i = -1;
    while( !snd_card_next( &i ) && i >= 0 )
    {
        char *name = nullptr;
        char *long_name = nullptr;

        snd_card_get_name( i, &name );
        snd_card_get_longname( i, &long_name );

        free( name );
        free( long_name );

        snd_ctl_t *ctl = nullptr;
        sprintf( card_id, "hw:%d", i );
        snd_ctl_open( &ctl, card_id, 0 );

        snd_ctl_card_info_t *cinfo = nullptr;
        snd_ctl_card_info_alloca( &cinfo );
        snd_ctl_card_info( ctl, cinfo );

        const char* cid = snd_ctl_card_info_get_id( cinfo );
        const char* cname = snd_ctl_card_info_get_name( cinfo );

        snd_pcm_info_t *pcm = nullptr;
        snd_pcm_info_alloca( &pcm );

        int j =-1;
        while( !snd_ctl_pcm_next_device( ctl, &j ) && j >= 0 )
        {
            snd_pcm_info_set_device( pcm, j );
            snd_pcm_stream_t strm = SND_PCM_STREAM_CAPTURE;
            snd_pcm_info_set_stream( pcm, strm );

            if( snd_ctl_pcm_info(ctl,pcm) < 0 )
            {
                continue;
            }

            const char *dev_id = snd_pcm_info_get_id( pcm);
            const char *dev_name = snd_pcm_info_get_name( pcm );

            if (count == alloc_size)
            {
                alloc_size += ALLOC_SLOTS;
                // need to realloc
                buffer = reinterpret_cast< ALSADevice *> (realloc (buffer,
                                sizeof (ALSADevice) * alloc_size));
            }

            device = new (&buffer[count]) ALSADevice();
            device->card_i = i;
            device->pcm_i = j;
            sprintf(pcm_id, "hw:%d,%d", i, j);
            device->pcm_id = reinterpret_cast<char*>(malloc(strlen(pcm_id) + 1));
            strcpy(device->pcm_id, pcm_id);
            device->card_id = reinterpret_cast<char*>(malloc(strlen(card_id) + 1));
            strcpy(device->card_id, card_id);
            device->card_name = reinterpret_cast<char*>(malloc(strlen(cid) + 1));
            strcpy(device->card_name, cid);
            device->pcm_name = reinterpret_cast<char*>(malloc(strlen(dev_id) + 1));
            strcpy(device->pcm_name, dev_id);
            count ++;
        }
    }

    if( count > 0 )
    {
        *devices = buffer;
        *num_devices = count;
    }
    else
    {
        *devices = nullptr;
        *num_devices = 0;
        free( buffer );
    }
}

uint32_t AAPIALSAReader::get_num_devices()
{
    return m_numDevices;
}

const char *AAPIALSAReader::get_device_id(int index)
{
    if( index >= 0 && index < m_numDevices )
    {
        return m_devices[ index ].pcm_id;
    }
    return nullptr;
}

const char *AAPIALSAReader::get_device_name(int index)
{
    if( index >= 0 && index < m_numDevices )
    {
        return m_devices[ index ].pcm_name;
    }
    return nullptr;
}

bool AAPIALSAReader::is_format_supported(int index,
                                         enum AAPIAudioChannels channels,
                                         enum AAPIAudioSampleRate sample_rate,
                                         enum AAPIAudioSampleSize sample_size)
{
    int ret;
    snd_pcm_t *handle;
    snd_pcm_format_t format;
    snd_pcm_hw_params_t *params;

    if( index < 0 || index >= m_numDevices )
    {
        return false;
    }

    switch( sample_size )
    {
    case AUDIO_SSIZE_16:
        format = SND_PCM_FORMAT_S16_LE;
        break;
    case AUDIO_SSIZE_24:
        format = SND_PCM_FORMAT_S24_LE;
        break;
    default:
        return false;
    }

    ret = snd_pcm_open( &handle, m_devices[index].pcm_id,
                       SND_PCM_STREAM_CAPTURE, 0 );
    if( ret < 0 )
    {
        /* audio open error */;
        return false;
    }

    snd_pcm_hw_params_alloca( &params );

    ret = snd_pcm_hw_params_any( handle, params );
    if( ret < 0 )
    {
        /* Broken configuration for this PCM */
        snd_pcm_close( handle );
        return false;
    }

        ret = snd_pcm_hw_params_test_format(handle, params, format);
    if (ret == 0)
        ret = snd_pcm_hw_params_test_channels(handle, params, channels);
    if (ret == 0)
        ret = snd_pcm_hw_params_test_rate(handle, params, sample_rate, 0);

    snd_pcm_close(handle);

    return( ret == 0 );
}

int AAPIALSAReader::open(const char* device_name,
                         enum AAPIAudioChannels channels,
                         enum AAPIAudioSampleRate sample_rate,
                         enum AAPIAudioSampleSize sample_size,
                         uint32_t nsamples)
{
    if( m_source )
    {
        /* Invalid state; must be closed before call. */
        return AAPIAUDIO_E_ALREADY_OPEN;
    }

    aapi_ptr<ALSASource> source(ALSASource::create(false));
    int ret;

    if( !source )
    {
        /* Unable to allocate memory. */
        return AAPI_E_OUT_OF_MEMORY;
    }

    ret = ALSASource::open(source, device_name,
                           static_cast< uint32_t >(channels),
                           static_cast< uint32_t >(sample_rate),
                           static_cast< uint32_t >(sample_size),
                           nsamples);
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    m_source = source.detach();

    return 0;
}

void AAPIALSAReader::close()
{
    if( m_source )
    {
        ALSASource::close(m_source);
        m_source->release();
        m_source = nullptr;
    }
}

int AAPIALSAReader::start(AAPIAudioReaderEvents* callback)
{
    int ret;

    ret = ALSASource::start(m_source, callback);
    if (AAPI_FAILED(ret))
    {
        /* Failed to start alsa source. */
        return ret;
    }

    return 0;
}

void AAPIALSAReader::stop()
{
    if (m_source)
    {
        ALSASource::stop(m_source);
    }
}

} //namespace aapi
