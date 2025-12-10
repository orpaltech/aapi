/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2025 ORPAL Technology, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <alsa/asoundlib.h>
#include "audio/impl/alsa_reader.h"

#define BUFFER_PERIODS  256U
#define DRIVER_PERIODS  3U

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AlsaSource
///////////////////////////////////////////////////////////////////////////////
/// \brief The ALSASource class
///

class AlsaSource : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AlsaSource)

public:
    snd_pcm_t           *handle;

    /* from cmd line parameters */
    snd_pcm_format_t    format;
    uint32_t            channels;
    uint32_t            rate;
    uint32_t            period_bytes;  /* bytes in a period */

    /* derived from cmd line params */
    snd_pcm_uframes_t   period_frames; /* number of frames in period */
    uint32_t            bits_per_sample;
    uint32_t            bits_per_frame;

    /* from sound hardware */
    snd_pcm_uframes_t   buffer_frames;  /* size of buffer (in frames) */

    /* buffer data */
    char                *buffer;
    uint32_t            buff_periods;   /* number of periods in buffer */
    int                 read_pos;       /* in bytes */
    int                 clnt_pos;

    int                 read_periods;
    int                 clnt_periods;

    /* locking/threading */
    pthread_t           rtid, ctid;
    pthread_mutex_t     mutex;      /* for access to reader/writer pos */
    sem_t               sem;        /* for waking writer */

    volatile bool       complete;   /* complete flag*/

    AAPiAudioReaderEvents *callback;    /* consumer callback */

    AlsaSource()
        : handle(nullptr)
        , rtid(0)
        , ctid(0)
        , callback(nullptr)
        , buff_periods(BUFFER_PERIODS)
    { }

    ~AlsaSource()
    {
        close();
    }

    int open(const char *device, uint32_t channels, uint32_t sample_rate, uint32_t sample_size, uint32_t num_samples);
    void close();

    int start(AAPiAudioReaderEvents *callback);
    void stop();

    static void *read_thread(void *data);

    static void *client_thread(void *data);

    int set_params();

    static void xrun_handler(AlsaSource *alsa);

    /* read pcm data from the audio driver */
    static ssize_t pcm_read(AlsaSource *alsa, size_t count);

    /* for debug logging */
    static snd_output_t *log;
};


int AlsaSource::open(const char *device, uint32_t channels, uint32_t sample_rate, uint32_t sample_size, uint32_t num_samples)
{
    if ( handle != nullptr)
    {
        return AAPI_E_INVALID_STATE;
    }

    // Period size in bytes per one channel.
    uint32_t period_bytes;
    int ret;

    // configure
    switch( sample_size )
    {
    case 16:
        this->format = SND_PCM_FORMAT_S16_LE;
        period_bytes = num_samples * 2 /*sample size = 2 bytes*/;
        break;
    case 24:
        this->format = SND_PCM_FORMAT_S24_LE;
        period_bytes = num_samples * 4 /*sample size = 4 bytes*/;
        break;
    case 32:
        this->format = SND_PCM_FORMAT_S32_LE;
        period_bytes = num_samples * 4 /*sample size = 4 bytes*/;
        break;
    default:
        return AAPI_AUDIO_E_INVALID_PARAM;
    }

    this->channels = channels;
    this->rate = sample_rate;
    this->period_bytes = period_bytes * channels;

    // for debug
    ret = snd_output_stdio_attach( &log, stderr, 0 );

    // open the alsa source
    ret = snd_pcm_open( &handle, device, SND_PCM_STREAM_CAPTURE, 0);
    if ( ret < 0 )
    {
        return AAPI_AUDIO_E_OPEN_DEVICE;
    }

    // configure audio
    ret = set_params();
    if( ret < 0 )
    {
        snd_pcm_close( handle );
        handle = nullptr;
        return AAPI_AUDIO_E_INVALID_PARAM;
    }

    buffer = reinterpret_cast<char *>( malloc( this->period_bytes * this->buff_periods ));
    if( !buffer )
    {
        snd_pcm_close( handle );
        handle = nullptr;
        return AAPI_E_OUT_OF_MEMORY;
    }

    // init mutex
    pthread_mutex_init( &mutex, nullptr );

    return 0;
}

void AlsaSource::close()
{
    if( !handle )
    {
        return;
    }

    stop();

    /* cleanup */
    snd_pcm_nonblock( handle, 0 );
    snd_pcm_drain( handle );
    snd_pcm_close( handle );

    /* Reset the handle */
    handle = nullptr;

    /* Release buffer */
    free( buffer );

    /* destroy mutex */
    pthread_mutex_destroy( &mutex );

    // TODO: think about this
    //snd_output_close(alsa_data->log);
}

int AlsaSource::start(AAPiAudioReaderEvents *callback)
{
    int ret;

    if( rtid != 0 )
    {
        return 0;
    }

    /* Reset flags. */
    complete = false;

    /* Set callback */
    this->callback = callback;

    /* Reset all positions/counters */
    read_pos = 0;
    clnt_pos = 0;
    read_periods = 0;
    clnt_periods = 0;

    /* init semaphore */
    sem_init( &sem, 0, 0 );

    /* init threads */
    ret = pthread_create( &rtid, nullptr, read_thread, this );
    if( ret < 0 )
    {
        /* failed to create thread */
        return AAPI_E_CREATE_THREAD_FAILED;
    }

    /* Sleep 10msec*/
    usleep( 10000 );

    ret = pthread_create( &ctid, nullptr, client_thread, this );
    if( ret < 0 )
    {
        pthread_detach( rtid );
        rtid = 0;
        /* failed to create thread */
        return AAPI_E_CREATE_THREAD_FAILED;
    }

    return 0;
}

void AlsaSource::stop()
{
    void *rend,*cend;

    if( rtid == 0 )
    {
        return;
    }

    // Signal complete flag
    complete = true;

    // Wait for workers to finish
    pthread_join( rtid, &rend );
    pthread_join( ctid, &cend );

    rtid = 0;
    ctid = 0;

    // Destroy semaphore
    sem_destroy( &sem );
}

void *AlsaSource::read_thread(void *data)
{
    AlsaSource *alsa = reinterpret_cast< AlsaSource *>( data );
    sched_param sparam;
    int policy;

    // Alter thread priority
    pthread_getschedparam( pthread_self(), &policy, &sparam );
    sparam.sched_priority = sched_get_priority_max( policy );
    pthread_setschedparam( pthread_self(), policy, &sparam );

    // Exit the loop after complete flag is set
    while( !alsa->complete )
    {
        pcm_read( alsa, alsa->period_bytes );
    }

    return nullptr;
}

void *AlsaSource::client_thread(void *data)
{
    AlsaSource *src = reinterpret_cast<AlsaSource *>( data );
    uint32_t period_bytes_chan = src->period_bytes / src->channels;
    uint32_t bytes_per_frame = src->bits_per_frame / 8;
    uint32_t bytes_per_sample = src->bits_per_sample / 8;
    char **samples_chan =
            reinterpret_cast<char **>(alloca( sizeof(char*) * src->channels ));
    char *buffer = src->buffer + src->clnt_pos;
    sched_param sparam;
    int policy, val;
    uint32_t bytes, i;

    /* allocate buffer for every channel */
    for ( i = 0; i < src->channels; i++ )
    {
        samples_chan[ i ] = reinterpret_cast<char *>( alloca( period_bytes_chan ) );
    }

    // alter thread priority
    pthread_getschedparam( pthread_self(), &policy, &sparam );
    sparam.sched_priority = sched_get_priority_max( policy );
    pthread_setschedparam( pthread_self(), policy, &sparam );

    while( true )
    {
        // Wait for next frame to be avail
        sem_wait( &src->sem );

        // Pointer wrap ?
        if ( src->clnt_pos == src->period_bytes * src->buff_periods )
        {
            src->clnt_pos = 0;
            buffer = src->buffer;
        }

        // Consume whole period
        bytes = src->period_bytes;

        if ( src->callback )
        {
            // Split channels and notify separated buffers
            for ( i = 0; i < src->channels; i++ )
            {
                char *frame_data = buffer;
                char *sample_data = samples_chan[i];

                for ( uint f = 0; f < src->period_frames; f++ )
                {
                    // Copy single channel sample data
                    memcpy( sample_data, frame_data + i*bytes_per_sample, bytes_per_sample );
                    frame_data += bytes_per_frame;
                    sample_data += bytes_per_sample;
                }
            }

            src->callback->onAudioReaderData( samples_chan, src->channels, period_bytes_chan );
        }

        src->clnt_periods++;

        // inc consumer pos
        src->clnt_pos += bytes;

        // inc buffer pointer
        buffer = src->buffer + src->clnt_pos;

        // check how many periods are waiting to be consumed
        sem_getvalue( &src->sem, &val );

        // has consumer been overtaken by the reader ?
        if( val >= src->buff_periods )
        {
            // overwrite
        }
        if( val < src->buff_periods )
        {
        }

        if( val == 0 && src->complete )
        {
            break;
        }
    }

    // Writer finished
    return nullptr;
}

int AlsaSource::set_params()
{
    snd_pcm_hw_params_t *params;
    snd_pcm_sw_params_t *swparams;
    int ret;

    // set the audio parameters
    snd_pcm_hw_params_alloca( &params );
    snd_pcm_sw_params_alloca( &swparams );

    /* number of bits per sample e.g. 16 for S16_LE */
    bits_per_sample = static_cast< uint32_t >(
            snd_pcm_format_physical_width( format ));

    /* number of bits in alsa frame e.g. 32 for S16_LE stereo */
    bits_per_frame = bits_per_sample * channels;

    /* number of frames in a period (period bytes / frame bytes) */
    period_frames = period_bytes / (bits_per_frame >> 3);

    /* config Hardware params */
    ret = snd_pcm_hw_params_any( handle, params );
    if( ret < 0 )
    {
        /* Broken configuration for this PCM */
        return -ENODEV;
    }

    /* set pcm format to be interleaved (e.g. LRLRLR for stereo) */
    ret = snd_pcm_hw_params_set_access( handle, params,
                                        SND_PCM_ACCESS_RW_INTERLEAVED );
    if( ret < 0 )
    {
        /* Access type not available */
        return -EINVAL;
    }

    /* sample format e.g. 16 bit little endian S16_LE */
    ret = snd_pcm_hw_params_set_format( handle, params, format );
    if( ret < 0 )
    {
        /* Sample format non available */
        return -EINVAL;
    }

    /* number of channels */
    ret = snd_pcm_hw_params_set_channels( handle, params, channels );
    if( ret < 0 )
    {
        /* Channels count non available */
        return -EINVAL;
    }

    /* rate (or nearest) */
    ret = snd_pcm_hw_params_set_rate_near( handle, params, &rate, 0 );
    if( ret < 0 )
    {
        /* rate non available */
        return -EINVAL;
    }

    /* config hardware buffering */

    /* get max supported buffer size */
    ret = snd_pcm_hw_params_get_buffer_size_max( params, &buffer_frames );

    /* we want buffer to be atleast DRIVER_PERIODS in size */
    if( (buffer_frames / period_frames) < DRIVER_PERIODS )
    {
        /* ALSA buffer too small */
        return -EINVAL;
    }

    /* set required period size */
    ret = snd_pcm_hw_params_set_period_size( handle, params, period_frames, 0 );
    if( ret < 0 )
    {
        /* period size not available */
        return -EINVAL;
    }

    /* set required buffer size (or nearest)*/
    ret = snd_pcm_hw_params_set_buffer_size_near( handle, params, &buffer_frames );
    if( ret < 0 )
    {
        /* buffer size not available */
        return -EINVAL;
    }

    /* commit all above hardware audio parameters to driver */
    ret = snd_pcm_hw_params( handle, params );
    if( ret < 0 )
    {
        /* Unable to install hw params */
        return -EINVAL;
    }

    /* config software audio params */
    snd_pcm_sw_params_current( handle, swparams );

    ret = snd_pcm_sw_params_set_avail_min( handle, swparams, period_frames );
    if( ret < 0 )
    {
        /* failed to set avail min */
        return -EINVAL;
    }

    /* frames for alsa-lib/driver to buffer internally before starting */
    ret = snd_pcm_sw_params_set_start_threshold( handle, swparams, 1 );
    if( ret < 0 )
    {
        /* failed to set start threshold */
        return -EINVAL;
    }

    /* if free frames >= buffer frames then stop */
    ret = snd_pcm_sw_params_set_stop_threshold( handle, swparams, buffer_frames );
    if( ret < 0 )
    {
        /* failed to set stop threshold */
        return -EINVAL;
    }

    /* commit the software params to alsa-lib */
    ret = snd_pcm_sw_params( handle, swparams );
    if( ret < 0 )
    {
        /* unable to install sw params */
        snd_pcm_sw_params_dump( swparams, log );
        return -EINVAL;
    }

    snd_pcm_dump( handle, log );
    return 0;
}

void AlsaSource::xrun_handler(AlsaSource *alsa)
{
    /* Handle device overrun */
    snd_pcm_recover( alsa->handle, -EPIPE, 0 );
}

/* read pcm data from the audio driver */
ssize_t AlsaSource::pcm_read(AlsaSource *alsa, size_t count)
{
    char *data = alsa->buffer + alsa->read_pos;
    ssize_t size;
    ssize_t result = 0;
    snd_pcm_uframes_t frames;
    int wait, ready_periods, overwrite = 0;

    /* check how many periods are free for reader */
    sem_getvalue( &alsa->sem, &ready_periods );

    /* has we overtaken the writer ?? */
    if (ready_periods >= (alsa->buff_periods - 1))
    {
        /*
         * We have many choices here (depending on our policy):
         * 1. Restart capture.
         * 2. Discard current period and wait for writer to catch up.
         * 3. Discard/flush writer periods.
         *
         * We choose option 2.
         */
        overwrite = 1;
    }

    /* change count from bytes to frames */
    frames = count / ( alsa->bits_per_frame >> 3 );

    /* read a number of frames from the driver */
    while ( frames > 0 )
    {
        size = snd_pcm_readi( alsa->handle, data, frames );

        if (size == -EAGAIN ||
            (size >= 0 && size < frames))
        {
            /* wait 1000ms max */
            wait = snd_pcm_wait( alsa->handle, 1000 );
            if (wait <= 0)
            {
                /* wait timeout/error */
            }
        }
        else if ( size == -EPIPE )
        {
            /* device overrun */
            xrun_handler( alsa );
        }
        else if ( size == -ESTRPIPE )
        {
            /* suspend(); */
        }
        else if ( size < 0 )
        {
            /* read error */
        }

        /* still have data to read ? */
        if ( size > 0 )
        {
            result += size;
            frames -= size;
            data += size * alsa->bits_per_frame / 8;
        }
    }

    /* return if we are about to overwrite */
    if ( overwrite )
    {
        return result;
    }

    /* got frames, so update reader pointer */
    alsa->read_pos += count;

    /* buffer wrap ? */
    if ( alsa->read_pos >= alsa->period_bytes * alsa->buff_periods )
    {
        alsa->read_pos = 0;
    }

    /* tell writer we have frame */
    sem_post( &alsa->sem );
    alsa->read_periods++;

    return result;
}

IMPLEMENT_AAPI_OBJECT(AlsaSource)

snd_output_t *AlsaSource::log = nullptr;

///////////////////////////////////////////////////////////////////////////////
// class ALSADevice
///////////////////////////////////////////////////////////////////////////////

class AlsaDevice
{
public:
    AlsaDevice()
        : card_id(nullptr)
        , pcm_id(nullptr)
        , card_name(nullptr)
        , pcm_name(nullptr)
    { }

    ~AlsaDevice()
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
// class AAPiAlsaReader
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT(AAPiAlsaReader)

AAPiAlsaReader::AAPiAlsaReader()
    : m_source(nullptr)
    , m_devices(nullptr)
    , m_numDevices(0)
{
    load_devices( &m_devices, &m_numDevices );
}

AAPiAlsaReader::~AAPiAlsaReader()
{
    free( m_devices );
}

#define ALLOC_SLOTS  5

void AAPiAlsaReader::load_devices(AlsaDevice **devices, uint32_t *num_devices)
{
    AlsaDevice *devs;
    AlsaDevice *device;
    int count = 0;
    size_t alloc_size = ALLOC_SLOTS;
    char card_id[10]={0};
    char pcm_id[10]={0};

    devs = reinterpret_cast< AlsaDevice *> (
                malloc (sizeof (AlsaDevice) * alloc_size));

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
                devs = reinterpret_cast< AlsaDevice *> (realloc (devs,
                                sizeof (AlsaDevice) * alloc_size));
            }

            device = new (&devs[count]) AlsaDevice();
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
        *devices = devs;
        *num_devices = count;
    }
    else
    {
        *devices = nullptr;
        *num_devices = 0;
        free( devs );
    }
}

uint32_t AAPiAlsaReader::get_num_devices()
{
    return m_numDevices;
}

const char *AAPiAlsaReader::get_device_id(int index)
{
    if( index >= 0 && index < m_numDevices )
    {
        return m_devices[ index ].pcm_id;
    }
    return nullptr;
}

const char *AAPiAlsaReader::get_device_name(int index)
{
    if( index >= 0 && index < m_numDevices )
    {
        return m_devices[ index ].pcm_name;
    }
    return nullptr;
}

bool AAPiAlsaReader::is_format_supported(int dev_index, AAPiAudioChannels channels,
                                         AAPiAudioSampleRate sample_rate,
                                         AAPiAudioSampleSize sample_size)
{
    int ret;
    snd_pcm_t *handle;
    snd_pcm_format_t format;
    snd_pcm_hw_params_t *params;
    const AlsaDevice *dev;

    if( dev_index < 0 || dev_index >= m_numDevices )
    {
        return false;
    }

    /* audio device at the given index */
    dev = &m_devices[ dev_index ];

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

    ret = snd_pcm_open( &handle, dev->pcm_id,
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

        ret = snd_pcm_hw_params_test_format( handle, params, format );
    if (ret == 0)
        ret = snd_pcm_hw_params_test_channels( handle, params, channels );
    if (ret == 0)
        ret = snd_pcm_hw_params_test_rate( handle, params, sample_rate, 0 );

    snd_pcm_close( handle );

    return( ret == 0 );
}

int AAPiAlsaReader::open(const char* device_name, AAPiAudioChannels channels,
                         AAPiAudioSampleRate sample_rate,
                         AAPiAudioSampleSize sample_size,
                         uint32_t num_samples)
{
    int ret;
    AAPiPtr<AlsaSource> source;

    if( m_source )
    {
        return AAPI_E_INVALID_STATE;
    }

    source.attach( AlsaSource::create() );
    if( !source )
    {
        return AAPI_E_OUT_OF_MEMORY;
    }

    ret = source->open( device_name,
                        static_cast< uint32_t >(channels),
                        static_cast< uint32_t >(sample_rate),
                        static_cast< uint32_t >(sample_size),
                        num_samples);
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    m_source = source.detach();

    return 0;
}

void AAPiAlsaReader::close()
{
    if( m_source )
    {
        m_source->close();
        AAPI_DISPOSE(m_source);
    }
}

int AAPiAlsaReader::start(AAPiAudioReaderEvents *callback)
{
    int ret;

    ret = m_source->start( callback );
    if (AAPI_FAILED( ret ))
    {
        // failed to start alsa source
        return ret;
    }

    return 0;
}

void AAPiAlsaReader::stop()
{
    if( m_source )
    {
        m_source->stop();
    }
}

} //namespace aapi
