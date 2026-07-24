/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

#undef USE_PERIOD_COUNTERS

// NOTE:Keep our threads at default scheduler priority. It needs deep investigation otherwise.
#define READ_THREAD_PRIORITY 30
#define CLIENT_THREAD_PRIORITY 0

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AlsaSource
///////////////////////////////////////////////////////////////////////////////

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

    std::atomic<uint32_t> read_pos;     /* in bytes */
    std::atomic<bool>     complete;     /* thread exit flag */

    uint32_t            clnt_pos;       /* Only modified by client thread */

#ifdef USE_PERIOD_COUNTERS
    int                 read_periods;
    int                 clnt_periods;
#endif

    /* locking/threading */
    pthread_t           rtid, ctid;
    sem_t               sem;            /* for waking writer */

    AAPiAudioReaderEvents *callback;    /* consumer callback */

    AlsaSource() : handle(nullptr)
        , rtid(0), ctid(0)
        , callback(nullptr)
        , buff_periods(BUFFER_PERIODS)
        , read_pos(0)
        , complete(false)
        , clnt_pos(0)
    { }

    ~AlsaSource() {
        close();
    }

    AAPiError open(const char *device, uint8_t channels,
                   uint32_t sample_rate, uint8_t sample_size,
                   uint32_t period_samples);
    void close();

    uint32_t get_buffer_size_chan() const { return this->period_bytes / this->channels; }

    AAPiError start(AAPiAudioReaderEvents *callback);
    void stop();

    int set_params();

    bool is_complete() const { return complete.load(std::memory_order_acquire); }
    void set_complete(bool val) { complete.store(val, std::memory_order_release); }

    static void *read_thread(void *data);
    static void *client_thread(void *data);

    static void xrun_handler(AlsaSource *alsa);

    /* read pcm data from the audio driver */
    static ssize_t pcm_read(AlsaSource *alsa, size_t count);

    static int set_thread_priority(pthread_t tid, unsigned int priority);

    /* for debug logging */
    static snd_output_t *log;
};

IMPLEMENT_AAPI_OBJECT(AlsaSource)

snd_output_t *AlsaSource::log = nullptr;


AAPiError AlsaSource::open(const char *device, uint8_t channels,
                           uint32_t sample_rate, uint8_t sample_size,
                           uint32_t period_samples)
{
    if ( handle != nullptr) {
        return AAPI_E_INVALID_STATE;
    }

    uint32_t period_bytes_chan;

    switch( sample_size ) {
    case 16:
        this->format = SND_PCM_FORMAT_S16_LE;
        period_bytes_chan = period_samples * 2;
        break;
    case 24:
        this->format = SND_PCM_FORMAT_S24_LE;
        period_bytes_chan = period_samples * 4;
        break;
    case 32:
        this->format = SND_PCM_FORMAT_S32_LE;
        period_bytes_chan = period_samples * 4;
        break;
    default:
        return AAPI_AUDIO_E_INVALID_PARAM;
    }

    this->channels = channels;
    this->rate = sample_rate;
    this->period_bytes = period_bytes_chan * channels;

    int ret = snd_output_stdio_attach( &log, stderr, 0 );

    ret = snd_pcm_open( &handle, device, SND_PCM_STREAM_CAPTURE, 0);
    if ( ret < 0 ) {
        return AAPI_AUDIO_E_OPEN_DEVICE;
    }

    ret = set_params();
    if( ret < 0 ) {
        snd_pcm_close( handle );
        handle = nullptr;
        return AAPI_AUDIO_E_INVALID_PARAM;
    }

    buffer = static_cast<char *>( malloc( this->period_bytes * this->buff_periods ));
    if( !buffer ) {
        snd_pcm_close( handle );
        handle = nullptr;
        return AAPI_E_OUT_OF_MEMORY;
    }

    return AAPI_SUCCESS;
}

void AlsaSource::close()
{
    if( !handle ) {
        return;
    }

    stop();

    snd_pcm_nonblock( handle, 0 );
    snd_pcm_drain( handle );
    snd_pcm_close( handle );
    handle = nullptr;

    free( buffer );

    if( log ) {
        snd_output_close( log );
        log = nullptr;
    }
}

int AlsaSource::set_thread_priority(pthread_t tid, unsigned int priority)
{
    sched_param param;
    param.sched_priority = priority;

    int ret = pthread_setschedparam( tid, SCHED_FIFO, &param );
    if (ret != 0) {
        fprintf(stderr, "Warning: Failed to set SCHED_FIFO priority (Error %d).\n", ret);
    } else {
        printf("Success: Thread running at Real-Time Priority %d\n", priority);
    }
    return ret;
}

AAPiError AlsaSource::start(AAPiAudioReaderEvents *cb)
{
    if( rtid != 0 ) {
        return AAPI_SUCCESS;
    }

    set_complete( false);
    callback = cb;

    read_pos.store(0, std::memory_order_release);
    clnt_pos = 0;
#ifdef USE_PERIOD_COUNTERS
    read_periods = 0;
    clnt_periods = 0;
#endif

    sem_init( &sem, 0, 0 );

    int ret = pthread_create( &rtid, nullptr, read_thread, this );
    if( ret < 0 ) {
        return AAPI_E_CREATE_THREAD_FAILED;
    }

#if READ_THREAD_PRIORITY
    set_thread_priority( rtid, READ_THREAD_PRIORITY );
#endif

    usleep( 10'000 );

    ret = pthread_create( &ctid, nullptr, client_thread, this );
    if( ret < 0 ) {
        pthread_detach( rtid );
        rtid = 0;
        return AAPI_E_CREATE_THREAD_FAILED;
    }

#if CLIENT_THREAD_PRIORITY
    set_thread_priority( ctid, CLIENT_THREAD_PRIORITY );
#endif

    return AAPI_SUCCESS;
}

void AlsaSource::stop()
{
    if( rtid == 0 ) {
        return;
    }

    set_complete( true );

    if ( handle ) {
        snd_pcm_abort( handle );
    }

    sem_post( &sem );

    pthread_join( rtid, nullptr );
    pthread_join( ctid, nullptr );

    rtid = 0;
    ctid = 0;

    sem_destroy( &sem );
}

void *AlsaSource::read_thread(void *data)
{
    AlsaSource *alsa = static_cast< AlsaSource *>( data );

    while( !alsa->is_complete() ) {
        pcm_read( alsa, alsa->period_bytes );
    }

    return nullptr;
}

ssize_t AlsaSource::pcm_read(AlsaSource *alsa, size_t count)
{
    const uint32_t bytes_per_frame = alsa->bits_per_frame / 8;
    snd_pcm_sframes_t size;
    ssize_t result = 0;
    int wait;

    snd_pcm_uframes_t frames = count / bytes_per_frame;

    // Load current position safely using relaxed memory ordering
    uint32_t current_write_pos = alsa->read_pos.load(std::memory_order_relaxed);
    char *data = alsa->buffer + current_write_pos;

    while ( frames > 0 ) {
        size = snd_pcm_readi( alsa->handle, data, frames );

        if (size == -EAGAIN || (size >= 0 && size < frames)) {
            if (size > 0) {
                result += size;
                frames -= size;
                data += size * bytes_per_frame;
            }

            wait = snd_pcm_wait( alsa->handle, 1000 );
            if (wait <= 0) {
#if defined(__linux__)
                sched_yield();
#endif
                return result;
            }

        } else if ( size == -EPIPE ) {
            xrun_handler( alsa );
            return 0;

        } else if ( size < 0 ) {
            return size;

        } else {
            result += size;
            frames -= size;
            data += size * bytes_per_frame;
        }
    }

    // Determine the next target ring buffer sector completely lock-free
    uint32_t next_write_pos = current_write_pos + count;
    if ( next_write_pos >= alsa->period_bytes * alsa->buff_periods ) {
        next_write_pos = 0;
    }

    // Publish changes using release semantics to establish a clean memory barrier
    alsa->read_pos.store(next_write_pos, std::memory_order_release);

    sem_post( &alsa->sem );

#ifdef USE_PERIOD_COUNTERS
    alsa->read_periods++;
#endif

    return result;
}

void *AlsaSource::client_thread(void *data)
{
    AlsaSource *src = static_cast<AlsaSource *>(data);
    const uint32_t period_bytes_chan = src->get_buffer_size_chan();
    const uint32_t bytes_per_frame = src->bits_per_frame / 8;
    const uint32_t bytes_per_sample = src->bits_per_sample / 8;

    char **samples_chan = static_cast<char **>(malloc(sizeof(char*) * src->channels));
    for (uint i = 0; i < src->channels; i++) {
        samples_chan[i] = static_cast<char *>(malloc(period_bytes_chan));
    }

    while (true)
    {
        sem_wait( &src->sem );

        if ( src->is_complete()) {
            break;
        }

        // Instantly check for backlog sizes before processing memory frames
        int val;
        sem_getvalue( &src->sem, &val );

        if (val >= static_cast<int>(src->buff_periods)) {
            // Drain outstanding notifications to instantly catch up
            for (int i = 0; i < val - 1; ++i) {
                sem_trywait(&src->sem);
            }

            // Snap data position safely behind the atomic pointer using acquire semantics
            uint32_t current_read_pos = src->read_pos.load(std::memory_order_acquire);
            if (current_read_pos >= src->period_bytes) {
                src->clnt_pos = current_read_pos - src->period_bytes;
            } else {
                src->clnt_pos = (src->period_bytes * src->buff_periods) - src->period_bytes;
            }

            // Inform calibration DSP routines to ignore this corrupt/skipped sweep step
            if (src->callback) {
                src->callback->onAudioBufferOverrun();
            }
        }

        if (src->clnt_pos >= (src->period_bytes * src->buff_periods)) {
            src->clnt_pos = 0;
        }

        char *buffer = src->buffer + src->clnt_pos;

        // Perform de-interleaving transformations
        if (src->callback) {
            if (src->format == SND_PCM_FORMAT_S24_LE || src->format == SND_PCM_FORMAT_S32_LE) {
                int32_t *frame_data = reinterpret_cast<int32_t *>(buffer);
                for (uint i = 0; i < src->channels; i++) {
                    int32_t *sample_data = reinterpret_cast<int32_t*>(samples_chan[i]);
                    for (uint f = 0; f < src->period_frames; f++) {
                        sample_data[f] = frame_data[(f * src->channels) + i];
                    }
                }
            } else {
                if (bytes_per_sample == 2) { // 16-bit performance route (removes slow memcpy calls)
                    for (uint i = 0; i < src->channels; i++) {
                        int16_t *frame_data = reinterpret_cast<int16_t*>(buffer);
                        int16_t *sample_data = reinterpret_cast<int16_t*>(samples_chan[i]);
                        for (uint f = 0; f < src->period_frames; f++) {
                            sample_data[f] = frame_data[(f * src->channels) + i];
                        }
                    }
                } else { // Generalized fallback loop
                    for (uint i = 0; i < src->channels; i++) {
                        char *frame_data = buffer;
                        char *sample_data = samples_chan[i];
                        for (uint f = 0; f < src->period_frames; f++) {
                            memcpy(sample_data, frame_data + i * bytes_per_sample, bytes_per_sample);
                            frame_data += bytes_per_frame;
                            sample_data += bytes_per_sample;
                        }
                    }
                }
            }

            src->callback->onAudioReaderData(samples_chan, src->channels, period_bytes_chan);
        }

#ifdef USE_PERIOD_COUNTERS
        src->clnt_periods++;
#endif

        src->clnt_pos += src->period_bytes;

        if (val == 0 && src->is_complete()) {
            break;
        }
    }

    for (uint i = 0; i < src->channels; i++) {
        free(samples_chan[i]);
    }
    free(samples_chan);

    return nullptr;
}

int AlsaSource::set_params()
{
    snd_pcm_hw_params_t *params;
    snd_pcm_sw_params_t *swparams;

    // set the audio parameters
    snd_pcm_hw_params_alloca( &params );
    snd_pcm_sw_params_alloca( &swparams );

    // number of bits per sample e.g. 16 for S16_LE
    bits_per_sample = static_cast< uint32_t >(
        snd_pcm_format_physical_width( format ));

    // number of bits in alsa frame e.g. 32 bits will be used for S16_LE stereo
    bits_per_frame = bits_per_sample * channels;

    // number of frames in a period (period bytes / frame bytes)
    uint32_t bytes_per_frame = bits_per_frame / 8;
    period_frames = period_bytes / bytes_per_frame;

    // config Hardware params
    int ret = snd_pcm_hw_params_any( handle, params );
    if( ret < 0 ) {
        // broken configuration for this PCM
        return -ENODEV;
    }

    // set pcm format to be interleaved (e.g. LRLRLR for stereo)
    ret = snd_pcm_hw_params_set_access( handle, params,
                                       SND_PCM_ACCESS_RW_INTERLEAVED );
    if( ret < 0 ) {
        // Access type not available
        return -EINVAL;
    }

    // sample format e.g. 16 bit little endian S16_LE
    ret = snd_pcm_hw_params_set_format( handle, params, format );
    if( ret < 0 ) {
        // sample format non available
        return -EINVAL;
    }

    // number of channels
    ret = snd_pcm_hw_params_set_channels( handle, params, channels );
    if( ret < 0 ) {
        // channels count non available
        return -EINVAL;
    }

    // rate (or nearest)
    ret = snd_pcm_hw_params_set_rate_near( handle, params, &rate, 0 );
    if( ret < 0 ) {
        // rate non available
        return -EINVAL;
    }

    // config hardware buffering

    // get max supported buffer size
    ret = snd_pcm_hw_params_get_buffer_size_max( params, &buffer_frames );

    // we want buffer to be atleast DRIVER_PERIODS in size
    if( (buffer_frames / period_frames) < DRIVER_PERIODS ) {
        // ALSA buffer too small
        return -EINVAL;
    }

    // set required period size
    ret = snd_pcm_hw_params_set_period_size( handle, params, period_frames, 0 );
    if( ret < 0 ) {
        // period size not available
        return -EINVAL;
    }

    // set required buffer size (or nearest)
    ret = snd_pcm_hw_params_set_buffer_size_near( handle, params, &buffer_frames );
    if( ret < 0 ) {
        // buffer size not available
        return -EINVAL;
    }

    // commit all above hardware audio parameters to driver
    ret = snd_pcm_hw_params( handle, params );
    if( ret < 0 ) {
        // Unable to install hw params
        return -EINVAL;
    }

    // config software audio params
    snd_pcm_sw_params_current( handle, swparams );

    ret = snd_pcm_sw_params_set_avail_min( handle, swparams, period_frames );
    if( ret < 0 ) {
        // failed to set avail min
        return -EINVAL;
    }

    // frames for alsa-lib/driver to buffer internally before starting
    ret = snd_pcm_sw_params_set_start_threshold( handle, swparams, 1 );
    if( ret < 0 ) {
        // failed to set start threshold
        return -EINVAL;
    }

    // if free frames >= buffer frames then stop
    ret = snd_pcm_sw_params_set_stop_threshold( handle, swparams, buffer_frames );
    if( ret < 0 ) {
        // failed to set stop threshold
        return -EINVAL;
    }

    // commit the software params to alsa-lib
    ret = snd_pcm_sw_params( handle, swparams );
    if( ret < 0 ) {
        // unable to install sw params
        snd_pcm_sw_params_dump( swparams, log );
        return -EINVAL;
    }

    snd_pcm_dump( handle, log );
    return 0;
}

void AlsaSource::xrun_handler(AlsaSource *alsa)
{
    // Handle device overrun
    snd_pcm_recover( alsa->handle, -EPIPE, 0 );
}

///////////////////////////////////////////////////////////////////////////////
// class ALSADevice
///////////////////////////////////////////////////////////////////////////////

class AlsaDevice
{
public:
    AlsaDevice()
        : card_id(nullptr), pcm_id(nullptr), card_name(nullptr), pcm_name(nullptr) {}

    AlsaDevice(AlsaDevice&& other) noexcept
        : card_i(other.card_i), pcm_i(other.pcm_i), card_id(other.card_id)
        , pcm_id(other.pcm_id), card_name(other.card_name), pcm_name(other.pcm_name)
    {
        other.card_id = nullptr;
        other.pcm_id = nullptr;
        other.card_name = nullptr;
        other.pcm_name = nullptr;
    }

    AlsaDevice& operator=(AlsaDevice&& other) noexcept {
        if (this != &other) {
            // Free current instance strings to avoid leaks
            free(card_id);
            free(pcm_id);
            free(card_name);
            free(pcm_name);

            // Steal new assets
            card_i = other.card_i;
            pcm_i = other.pcm_i;
            card_id = other.card_id;
            pcm_id = other.pcm_id;
            card_name = other.card_name;
            pcm_name = other.pcm_name;

            // Null out source so its destructor does nothing
            other.card_id = nullptr;
            other.pcm_id = nullptr;
            other.card_name = nullptr;
            other.pcm_name = nullptr;
        }
        return *this;
    }

    // Explicitly disallow copying to prevent accidental double-frees
    AlsaDevice(const AlsaDevice& ) = delete;
    AlsaDevice& operator=(const AlsaDevice&) = delete;

    ~AlsaDevice() {
        free(card_id);
        free(pcm_id);
        free(card_name);
        free(pcm_name);
    }

    int card_i, pcm_i;
    char *card_id, *pcm_id, *card_name, *pcm_name;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiAlsaReader
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT(AAPiAlsaReader)

AAPiAlsaReader::AAPiAlsaReader()
    : m_devices(nullptr)
    , m_numDevices(0)
{
    load_devices( &m_devices, &m_numDevices );
}

AAPiAlsaReader::~AAPiAlsaReader()
{
    free_devices( m_devices, m_numDevices );
}

void AAPiAlsaReader::free_devices(AlsaDevice*& devs, uint32_t& count)
{
    delete[] devs;
    devs = nullptr;
    count = 0;
}

#define ALLOC_BUNCH 5

void AAPiAlsaReader::load_devices(AlsaDevice **devices, uint32_t *num_devices)
{
    if (!devices || !num_devices)
        return;

    uint32_t count = 0;
    size_t alloc_size = ALLOC_BUNCH;

    char card_id[32] = {0};
    char pcm_id[32] = {0};

    // Use standard C++ array allocation
    AlsaDevice *devs = new(std::nothrow) AlsaDevice[alloc_size];
    if (!devs) {
        *devices = nullptr;
        *num_devices = 0;
        return;
    }

    int i = -1;
    while (snd_card_next(&i) == 0 && i >= 0)
    {
        snd_ctl_t *ctl = nullptr;
        snprintf(card_id, sizeof(card_id), "hw:%d", i);

        if (snd_ctl_open(&ctl, card_id, 0) < 0) {
            continue;
        }

        snd_ctl_card_info_t *cinfo = nullptr;
        snd_ctl_card_info_alloca(&cinfo);

        if (snd_ctl_card_info(ctl, cinfo) < 0) {
            snd_ctl_close(ctl);
            continue;
        }

        const char* cid = snd_ctl_card_info_get_id(cinfo);

        snd_pcm_info_t *pcm = nullptr;
        snd_pcm_info_alloca(&pcm);

        int j = -1;
        while (snd_ctl_pcm_next_device(ctl, &j) == 0 && j >= 0)
        {
            snd_pcm_info_set_device(pcm, j);
            snd_pcm_info_set_stream(pcm, SND_PCM_STREAM_CAPTURE);

            if (snd_ctl_pcm_info(ctl, pcm) < 0) {
                continue;
            }

            const char *dev_id = snd_pcm_info_get_id(pcm);

            if (count == alloc_size) {
                alloc_size += ALLOC_BUNCH;

                AlsaDevice *tmp = new(std::nothrow) AlsaDevice[alloc_size];
                if (!tmp) {
                    snd_ctl_close(ctl);
                    free_devices(devs, count);
                    *devices = nullptr;
                    *num_devices = 0;
                    return;
                }

                // C++ move assignment transfers the inner pointers seamlessly
                for (uint32_t k = 0; k < count; k++) {
                    tmp[k] = std::move(devs[k]);
                }

                delete[] devs;
                devs = tmp;
            }

            AlsaDevice &device = devs[count];
            device.card_i = i;
            device.pcm_i = j;

            snprintf(pcm_id, sizeof(pcm_id), "hw:%d,%d", i, j);

            device.pcm_id = strdup(pcm_id);
            device.card_id = strdup(card_id);
            device.card_name = strdup(cid ? cid : "");
            device.pcm_name = strdup(dev_id ? dev_id : "");

            count++;
        }

        snd_ctl_close(ctl);
    }

    if (count > 0) {
        *devices = devs;
        *num_devices = count;
        return;
    }

    // Clean up empty container allocation if no devices were matching
    delete[] devs;
    *devices = nullptr;
    *num_devices = 0;
}

AAPiString AAPiAlsaReader::get_device_id(int index)
{
    if( index >= 0 && index < m_numDevices ) {
        return AAPiString{ m_devices[ index ].pcm_id };
    }
    return AAPiString();
}

AAPiString AAPiAlsaReader::get_device_name(int index)
{
    if( index >= 0 && index < m_numDevices ) {
        return AAPiString{ m_devices[ index ].pcm_name };
    }
    return AAPiString();
}

bool AAPiAlsaReader::is_format_supported(int dev_index, Channels channels, SampleRate sample_rate, SampleSize sample_size)
{
    if (dev_index < 0 || dev_index >= m_numDevices) {
        return false;   // Invalid index
    }

    unsigned int alsa_channels = static_cast<unsigned int>(channels);
    unsigned int alsa_rate     = static_cast<unsigned int>(sample_rate);
    snd_pcm_format_t alsa_format;

    switch (sample_size) {
    case SampleSize::_16:
        alsa_format = SND_PCM_FORMAT_S16_LE;
        break;
    case SampleSize::_24:
        alsa_format = SND_PCM_FORMAT_S24_LE;
        break;
    case SampleSize::_32:
        alsa_format = SND_PCM_FORMAT_S32_LE;
        break;
    default:
        return false;
    }

    snd_pcm_t *handle = nullptr;
    const AlsaDevice *dev = &m_devices[dev_index];

    int ret = snd_pcm_open(&handle, dev->pcm_id, SND_PCM_STREAM_CAPTURE, 0);
    if (ret < 0) {
        return false;
    }

    // Allocate and initialize hardware parameters configuration space
    snd_pcm_hw_params_t *params = nullptr;
    snd_pcm_hw_params_alloca(&params);

    ret = snd_pcm_hw_params_any(handle, params);
    if (ret < 0) {
        snd_pcm_close(handle);
        return false;
    }

    // Test capabilities individually using explicit early exits
    if (snd_pcm_hw_params_test_format(handle, params, alsa_format) < 0) {
        snd_pcm_close(handle);
        return false;
    }

    if (snd_pcm_hw_params_test_channels(handle, params, alsa_channels) < 0) {
        snd_pcm_close(handle);
        return false;
    }

    // Note: 0 value means exact match requirement for ALSA direction parameter
    if (snd_pcm_hw_params_test_rate(handle, params, alsa_rate, 0) < 0) {
        snd_pcm_close(handle);
        return false;
    }

    // Everything passed
    snd_pcm_close(handle);
    return true;
}


AAPiError AAPiAlsaReader::open(const AAPiString& dev_id, Channels channels,
                               SampleRate sample_rate, SampleSize sample_size,
                               uint32_t period_samples)
{
    if( m_source != nullptr ) {
        return AAPI_E_INVALID_STATE;
    }

    AAPiPtr<AlsaSource> source( AlsaSource::create(),false );
    if( !source ) {
        return AAPI_E_OUT_OF_MEMORY;
    }

    auto ret = source->open( dev_id, static_cast<uint8_t>(channels),
                            static_cast<uint32_t>(sample_rate),
                            static_cast<uint8_t>(sample_size),
                            period_samples);
    if( AAPI_FAILED(ret) ) {
        return ret;
    }

    m_source = std::move(source);
    return 0;
}

void AAPiAlsaReader::close()
{
    if( m_source ) {
        m_source->close();
        m_source = nullptr;
    }
}

uint32_t AAPiAlsaReader::get_buffer_size_chan() const
{
    return m_source ? m_source->get_buffer_size_chan() : 0;
}

AAPiError AAPiAlsaReader::start(AAPiAudioReaderEvents *callback)
{
    auto ret = m_source->start( callback );
    if (AAPI_FAILED( ret )) {
        // failed to start alsa source
        return ret;
    }

    return AAPI_SUCCESS;
}

void AAPiAlsaReader::stop()
{
    if( m_source ) {
        m_source->stop();
    }
}

} //namespace aapi
