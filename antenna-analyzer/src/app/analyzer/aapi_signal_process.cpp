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

#include <math.h>
#include <fftw3.h>
#include "aapi_signal_process.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPiSignalProcessor
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(AAPiSignalProcessor)

AAPiSignalProcessor::AAPiSignalProcessor()
    : m_reader(AAPiAudioReader::create())
    , m_fft_out(nullptr)
    , m_fft_wnd(nullptr)
    , m_plan(nullptr)
    , m_config(nullptr)
{
    m_raw_inp[DSP_V_CHANNEL] = nullptr;
    m_raw_inp[DSP_I_CHANNEL] = nullptr;

    m_fft_inp[DSP_V_CHANNEL] = nullptr;
    m_fft_inp[DSP_I_CHANNEL] = nullptr;

    m_fft_mags[DSP_V_CHANNEL] = nullptr;
    m_fft_mags[DSP_I_CHANNEL] = nullptr;
}

AAPiSignalProcessor::~AAPiSignalProcessor()
{
    stop();

    AAPI_DISPOSE(m_reader);
    AAPI_DISPOSE(m_config);
}

void AAPiSignalProcessor::releaseBuffers()
{
    for (int i=0; i< NUM_DSP_CHANNELS; i++)
    {
        fftw_free( m_raw_inp[i] );
        m_raw_inp[i] = nullptr;

        fftw_free( m_fft_inp[i] );
        m_fft_inp[i] = nullptr;

        fftw_free( m_fft_mags[i] );
        m_fft_mags[i] = nullptr;
    }

    fftw_free( m_fft_out );
    m_fft_out = nullptr;

    fftw_free( m_fft_wnd );
    m_fft_wnd = nullptr;
}

double AAPiSignalProcessor::mag2db(double mag)
{
    return 20. * std::log10( mag );
}

void AAPiSignalProcessor::setBlackman(double *wnd, uint32_t num)
{
    for ( uint i = 0; i < num; i++ )
    {
        wnd[ i ] = (
            0.42659071 -
            0.49656062 * std::cos( ( 2.0 * M_PI * i ) / ( num - 1 ) ) +
            0.07684867 * std::cos( ( 4.0 * M_PI * i ) / ( num - 1 ) )
        );
    }

    // w(n) = 0.42 - 0.5 * cos(2πn / (N-1)) + 0.08 * cos(4πn / (N-1))
}

AAPiComplex AAPiSignalProcessor::calcMagnitude(int channel)
{
    AAPiComplex *bf;
    double mag, phase, pwr;
    uint32_t bin;

    bin = m_config->get_dsp_fft_if_bin();
    pwr = 0.;

    /*
     * Calculate signal magnitude for a bin considering +/- 2 bins from maximum
     */
    for (int i = bin - 2; i <= bin + 2; i++)
    {
        mag = m_fft_mags [channel][i];
        pwr += std::pow ( mag, 2 );
    }

    mag = sqrt( pwr );

    bf = &m_fft_out[ bin ];
    phase = std::atan2( bf->imag(), bf->real() );

    return AAPiComplex( mag, phase );
}

int32_t readSample(AAPiAudioSampleSize samp_size, int index, char *buffer)
{
    int32_t samp_val = 0;

    if (samp_size == AUDIO_SSIZE_16)
    {
        /* 16bit, signed integer */
        int16_t *samp_buff = reinterpret_cast<int16_t *> (buffer);
        samp_val = samp_buff[index];
    }
    else if (samp_size == AUDIO_SSIZE_24)
    {
        /* 32bit, signed integer */
        int32_t *samp_buff = reinterpret_cast<int32_t *> (buffer);
        samp_val = samp_buff[index];
    }

    return samp_val;
}

void AAPiSignalProcessor::processAudioBuffer(int channel, char *buffer, uint32_t /*buff_size*/)
{
    fftw_plan plan = nullptr; //(fftw_plan) m_plan; // TODO: remove
    AAPiAudioSampleSize samp_size
            = static_cast<AAPiAudioSampleSize>( m_config->get_dsp_sample_size() );
    uint32_t num_samples, num_fft_pts;

    num_samples = m_config->get_dsp_num_samples();

    // copy audio samples in the input array 
    for (int i = 0; i < num_samples; i++)
    {
        m_raw_inp [channel][i] = readSample( samp_size, i, buffer );
        m_fft_inp [channel][i] = ( m_fft_wnd[i] * m_raw_inp [channel][i] );
    }

    // TODO: optimization attempt
    // can we create a single plan for multiple frames ?
    //if ( plan == nullptr ) {

    /*
     * Create a new FFTW plan for "real-to-complex" 1-dim transform
     * According to FFTW3 documenation, direct cast from std::complex to fftw_complex is allowed
     */
    plan = fftw_plan_dft_r2c_1d( num_samples, m_fft_inp [ channel ],
                                reinterpret_cast<fftw_complex *>( m_fft_out ),
                                FFTW_ESTIMATE );

        // Store prepared plan
        //m_plan = plan;
    //}

    /* execute FFT */
    fftw_execute( plan );

    /* release memory, associated with FFTW plan */
    fftw_destroy_plan( plan );

    // number of FFT points
    num_fft_pts = m_config->get_dsp_fft_num_pts();

    for (int i = 0; i < num_fft_pts; i++)
    {
        /* normalize output array */
        m_fft_mags [channel][i] = std::abs( m_fft_out[i] ) / ( num_fft_pts );
    }

    /*
     * Calculate signal magnitude for a bin considering +/- 2 bins from maximum
     */
    m_fft_xmag [channel] = calcMagnitude( channel );
}

void AAPiSignalProcessor::onAudioReaderData(char **buffers, uint32_t num_channels, uint32_t buf_size)
{
    if (m_callbacks.size() == 0)
    {
        return;
    }

    if (num_channels < 2)
    {
        return;
    }

    AAPiSignalProcessEvents *cb;
    bool processing = false;
    uint32_t num_samples, num_fft_pts;

    /* number of samples */
    num_samples = m_config->get_dsp_num_samples();
    /* number of FFT points */
    num_fft_pts = m_config->get_dsp_fft_num_pts();

    /* check if at least one callback is enabled */
    for (int i = 0; i < m_callbacks.size(); i++)
    {
        cb = m_callbacks[i];

        if (cb && cb->isProcessingSignal())
        {
            processing = true;
            break;
        }
    }

    if (! processing)
        return;

    for (int ch = 0; ch < num_channels; ch++)
    {
        processAudioBuffer( ch, buffers[ch], buf_size );
    }

    for (int i = 0; i < m_callbacks.size(); i++)
    {
        cb = m_callbacks[i];

        if (cb && cb->isProcessingSignal())
        {
            /* run callbacks */
            cb->onSignalProcessRaw( m_raw_inp, num_channels, num_samples );
            cb->onSignalProcessFFT( m_fft_mags, num_channels, num_fft_pts );
            cb->onSignalProcessMags( m_fft_xmag, num_channels );
        }
    }
}

int AAPiSignalProcessor::start()
{
    AAPiAudioSampleRate sample_rate;
    AAPiAudioSampleSize sample_size;
    QString dev_name;
    uint32_t num_samples;
    int ret, i, dev_index = -1;

    // Read configuration parameters for ADC
    sample_rate = static_cast<AAPiAudioSampleRate>( m_config->get_dsp_sample_rate() );
    sample_size = static_cast<AAPiAudioSampleSize>( m_config->get_dsp_sample_size() );

    dev_name = m_config->get_audio_input_device();
    num_samples = m_config->get_dsp_num_samples();

    for (i = 0; i < m_reader->get_num_devices(); i++)
    {
        if (dev_name == m_reader->get_device_id( i ))
        {
            if (! m_reader->is_format_supported( i, AUDIO_CHANNELS_2,
                                                sample_rate, sample_size ))
            {
                return AAPI_DSP_E_ADC_UNSUPPORT_FORMAT;
            }
            dev_index = i;
            break;
        }
    }
    if ( dev_index < 0 )
    {
        return AAPI_DSP_E_ADC_DEVICE_NOT_FOUND;
    }

    // Open audio reader
    ret = m_reader->open( dev_name.toLatin1(), AUDIO_CHANNELS_2,
                          sample_rate, sample_size, num_samples );
    if (AAPI_FAILED( ret ))
    {
        // failed to open audio reader.
        return ret;
    }

    // Allocate required buffers
    for (i = 0; i< NUM_DSP_CHANNELS; i++)
    {
        m_raw_inp[i] = fftw_alloc_real( num_samples );
        m_fft_inp[i] = fftw_alloc_real( num_samples );
        m_fft_mags[i] = fftw_alloc_real( num_samples / 2 );

        // Check if any of memory allocations has failed
        if (!m_raw_inp[i] || !m_fft_inp[i] || !m_fft_mags[i])
        {
            releaseBuffers();
            return AAPI_E_OUT_OF_MEMORY;
        }
    }
    m_fft_out = reinterpret_cast<AAPiComplex *> ( fftw_alloc_complex (( num_samples / 2 ) + 1));
    m_fft_wnd = fftw_alloc_real( num_samples );

    // Check if any of memory allocations has failed
    if (!m_fft_out || !m_fft_wnd)
    {
        releaseBuffers();
        return AAPI_E_OUT_OF_MEMORY;
    }

    // Prepare blackman window 
    setBlackman( m_fft_wnd, num_samples );

    // Start audio reader, providing a callback pointer
    ret = m_reader->start( this );
    if (AAPI_FAILED( ret ))
    {
        // Failed to start audio reader.
        m_reader->close();
        releaseBuffers();
        return ret;
    }

    return 0;
}

void AAPiSignalProcessor::stop()
{
    m_reader->close();

    releaseBuffers();
}

void AAPiSignalProcessor::addCallback(AAPiSignalProcessEvents *cb)
{
    m_callbacks.append(cb);
}

} //namespace aapi
