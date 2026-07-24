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

#include "aapi_signal_process.h"
#include "utils/aapi_math_utils.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPiSignalProcessor
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(AAPiSignalProcessor)

AAPiSignalProcessor::AAPiSignalProcessor()
    : m_reader(AAPiAudioReader::create(false))
    , m_fft_out(nullptr)
    , m_fft_wnd(nullptr)
    , m_plan(nullptr)
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
}

void AAPiSignalProcessor::releaseBuffers()
{
    if( m_plan ) {
        fftw_plan plan = static_cast<fftw_plan>(m_plan);
        fftw_destroy_plan( plan );
        m_plan = nullptr;
    }

    for (int i=0; i< N_DSP_CHANNELS; i++) {
        fftw_free( m_raw_inp[i] );
        fftw_free( m_fft_inp[i] );
        fftw_free( m_fft_mags[i] );

        m_raw_inp[i] = nullptr;
        m_fft_inp[i] = nullptr;
        m_fft_mags[i] = nullptr;
    }

    fftw_free( m_fft_out );
    fftw_free( m_fft_wnd );

    m_fft_out = nullptr;
    m_fft_wnd = nullptr;
}

AAPiReal AAPiSignalProcessor::mag2db(AAPiReal mag)
{
    // Protect against log10(0) which returns NaN/Infinity
    if (mag < 1e-10) {
        return -120.0;
    }

    // Because the input is normalized to 1.0,
    // a maximum signal will return exactly 0.0 dBFS.
    return 20. * std::log10(mag);
}

AAPiReal AAPiSignalProcessor::setBlackman(AAPiReal *wnd, uint32_t num)
{
    AAPiReal sum = 0.0;

    for ( uint i = 0; i < num; i++ ) {
        wnd[ i ] = (
            0.42659071 -
            0.49656062 * std::cos( ( 2.0 * math::pi * i ) / ( num - 1 ) ) +
            0.07684867 * std::cos( ( 4.0 * math::pi * i ) / ( num - 1 ) )
            );
        sum += wnd[ i ]; // Accumulate the total weight of the window
    }

    // The Coherent Gain of any window is mathematically defined as the average value
    // of all the window's weights.
    // For a theoretical Blackman window of infinite length, this sum evaluates exactly
    // to the window's DC coefficient: 0.42659071 (which matches the first number inside
    // setBlackman function).
    // For real digital windows of finite length, the actual average changes slightly
    // depending on the exact value of N due to discrete sampling artifacts.
    //
    AAPiReal window_gain = sum / num;
    return window_gain;
}

AAPiComplex AAPiSignalProcessor::calcMagnitude(int channel)
{
    // Force the target bin to signed int to safely handle negative loops without underflows
    int bin = static_cast<int>(m_config->get_dsp_fft_if_bin());
    uint32_t num_samples = m_config->get_dsp_num_samples();
    int num_fft_pts = static_cast<int>(m_config->get_dsp_fft_num_pts());

    AAPiReal pwr = 0.0;
    uint num_used_bins = 0;

    // Loop across the 5-bin cluster around your target IF channel carrier
    for (int i = bin - 2; i <= bin + 2; i++) {

        // Enforce boundary validation strictly against the actual r2c buffer size limit
        if (i >= 0 && i < num_fft_pts) {
            AAPiReal fft_mag = m_fft_mags[channel][i];
            pwr += MathUtils::sqr(fft_mag);
            num_used_bins++;
        }
    }

    // Prevent division-by-zero if 'bin' is completely out of range
    AAPiReal mag = 0.0;
    if (num_used_bins > 0) {
        mag = std::sqrt(pwr / num_used_bins);
    }

    // Ensure 'bin' itself is within bounds before dereferencing m_fft_out
    AAPiReal phase = 0.0;
    if (bin >= 0 && bin < num_fft_pts) {
        AAPiComplex *bf = &m_fft_out[bin];
        phase = std::atan2(bf->imag(), bf->real());
    } else {
        // Target bin is out of bounds for phase calculation
    }

    return AAPiComplex(mag, phase);
}

AAPiReal AAPiSignalProcessor::readSample(int index, char *buffer)
{
    int32_t sample_val = 0;
    uint32_t max_val;

    switch (m_config->get_dsp_sample_size()) {
    case 24: {
        // Read as unsigned to safely perform bitwise manipulation
        uint32_t *raw_samples = reinterpret_cast<uint32_t *> (buffer);
        uint32_t raw_val = raw_samples[index];

        // Shift left by 8 to align the 24th bit with the 32nd bit
        // Cast to signed int32_t before shifting right
        // This forces a safe, deterministic arithmetic right shift (sign extension)
        sample_val = static_cast<int32_t>(raw_val << 8) >> 8;

        // Exact normalization to [-1.0, 1.0]
        if (sample_val >= 0) {
            max_val = 8388607.0; // 2^23 - 1
        } else {
            max_val = 8388608.0; // 2^23
        }
        break;
    }

    case 16: {
        // Read directly using a signed 16-bit pointer
        int16_t *raw_samples = reinterpret_cast<int16_t *> (buffer);

        // Native C++ casting handles sign-extension perfectly without manual bit shifts
        sample_val = static_cast<int32_t>(raw_samples[index]);

        // Exact normalization to [-1.0, 1.0]
        if (sample_val >= 0) {
            max_val = 32767.0;  // 2^15 - 1
        } else {
            max_val = 32768.0;  // 2^15
        }
        break;
    }

    default:
        return 0.0;
    }

    // Divide by maximum possible positive 32-bit signed integer value
    return static_cast<AAPiReal>(sample_val) / max_val;
}

void AAPiSignalProcessor::processAudioBuffer(int channel, char *buffer, uint32_t /*buff_size*/)
{
    uint32_t num_samples = m_config->get_dsp_num_samples();

    // copy samples into the input FFT array
    for (uint i = 0; i < num_samples; i++) {
        m_raw_inp [channel][i] = readSample( i, buffer );
        m_fft_inp [channel][i] = ( m_fft_wnd[i] * m_raw_inp [channel][i] );
    }

    /*
     * Create a new FFTW plan for "real-to-complex" 1-dim transform
     * According to FFTW3 documenation, direct cast from std::complex to fftw_complex is allowed
     */
    /*fftw_plan plan = nullptr;
    plan = fftw_plan_dft_r2c_1d( num_samples, m_fft_inp [ channel ],
                                reinterpret_cast<fftw_complex *>( m_fft_out ),
                                FFTW_ESTIMATE );

    // execute FFT
    fftw_execute( plan );
    // release memory, associated with FFTW plan
    fftw_destroy_plan( plan );*/

    fftw_plan plan = static_cast<fftw_plan>(m_plan);
    fftw_execute_dft_r2c ( plan, m_fft_inp [ channel ],
                         reinterpret_cast<fftw_complex *>( m_fft_out ) );


    //fftwf_execute_dft_r2c ()

    // number of FFT points
    uint32_t max_out_pts = m_config->get_dsp_fft_num_pts();

    for (uint i = 0; i < max_out_pts; i++) {
        //
        // Normalize by Blackman gain, and multiply by our digital pre-amp
        // to scale the raw numbers upward before they reach any callbacks.
        //
        AAPiReal normalized_mag = std::abs( m_fft_out[i] ) / ( max_out_pts * m_fft_wnd_gain );

        m_fft_mags [channel][i] = normalized_mag * m_config->get_dsp_digital_preamp();
    }

    // Calculate signal magnitude for a bin
    m_fft_xmag [channel] = calcMagnitude( channel );
}

void AAPiSignalProcessor::onAudioReaderData(char **buffers, uint32_t num_channels, uint32_t buf_size)
{
    if (m_callbacks.size() == 0) {
        return;
    }

    if (num_channels < N_DSP_CHANNELS) {
        return;
    }

    AAPiSignalProcessEvents *cb;
    bool processing = false;

    // check if at least one callback has enabled signal processing
    for (uint i = 0; i < m_callbacks.size(); i++) {
        cb = m_callbacks[i];
        processing = (cb && cb->is_signal_processing());
        if (processing) {
            break;
        }
    }

    if (! processing) {
        return;
    }

    uint32_t num_samples = m_config->get_dsp_num_samples();
    uint32_t max_fft_pts = m_config->get_dsp_fft_num_pts();

    for (uint ch = 0; ch < num_channels; ch++) {
        processAudioBuffer( ch, buffers[ch], buf_size );
    }

    for (uint i = 0; i < m_callbacks.size(); i++) {
        cb = m_callbacks[i];

        if (cb && cb->is_signal_processing()) {
            // run callbacks
            cb->onSignalProcessRaw( m_raw_inp, num_channels, num_samples );
            cb->onSignalProcessFFT( m_fft_mags, num_channels, max_fft_pts );
            cb->onSignalProcessMags( m_fft_xmag, num_channels );
        }
    }
}

void AAPiSignalProcessor::onAudioBufferOverrun()
{
    AAPiSignalProcessEvents *cb;

    for (uint i = 0; i < m_callbacks.size(); i++) {
        cb = m_callbacks[i];

        if (cb && cb->is_signal_processing()) {
            cb->onSignalProcessError(AAPI_DSP_E_ADC_BUFFER_OVERRUN);
        }
    }
}

AAPiError AAPiSignalProcessor::start()
{
    AAPiAudioSampleRate sample_rate;
    AAPiAudioSampleSize sample_size;
    int ret, dev_index = -1;

    // Read configuration parameters for ADC
    sample_rate = static_cast<AAPiAudioSampleRate>( m_config->get_dsp_sample_rate() );
    sample_size = static_cast<AAPiAudioSampleSize>( m_config->get_dsp_sample_size() );

    AAPiString device_id = m_config->get_audio_input_device();
    uint32_t num_samples = m_config->get_dsp_num_samples();
    uint32_t num_fft_pts = m_config->get_dsp_fft_num_pts();

    for (uint i = 0; i < m_reader->get_num_devices(); i++) {
        if (device_id == m_reader->get_device_id( i )) {
            if (! m_reader->is_format_supported( i, AAPiAudioChannels::Stereo,
                                               sample_rate, sample_size )) {
                return AAPI_DSP_E_ADC_UNSUPPORT_FORMAT;
            }
            dev_index = i;
            break;
        }
    }
    if ( dev_index < 0 ) {
        return AAPI_DSP_E_ADC_DEVICE_NOT_FOUND;
    }

    // Open audio reader
    ret = m_reader->open( device_id, AAPiAudioChannels::Stereo, sample_rate,
                         sample_size, num_samples );
    if (AAPI_FAILED( ret )) {
        // failed to open audio reader.
        return ret;
    }

    // Allocate required buffers
    for (uint i = 0; i < N_DSP_CHANNELS; i++) {

        m_raw_inp[i] = fftw_alloc_real( num_samples );
        m_fft_inp[i] = fftw_alloc_real( num_samples );

        // Allocate space for the DC component up to Nyquist frequency (+1)
        m_fft_mags[i] = fftw_alloc_real( num_fft_pts );

        if (!m_raw_inp[i] || !m_fft_inp[i] || !m_fft_mags[i]) {
            releaseBuffers();
            return AAPI_E_OUT_OF_MEMORY;
        }
    }
    m_fft_out = reinterpret_cast<AAPiComplex *> ( fftw_alloc_complex( num_fft_pts ));
    m_fft_wnd = fftw_alloc_real( num_samples );

    // Check if any of memory allocations has failed
    if (!m_fft_out || !m_fft_wnd) {
        releaseBuffers();
        return AAPI_E_OUT_OF_MEMORY;
    }

    // Prepare blackman window 
    m_fft_wnd_gain = setBlackman( m_fft_wnd, num_samples );

    /*
     * Create a new FFTW plan for "real-to-complex" 1-dim transform
     * According to FFTW3 documenation, direct cast from std::complex to fftw_complex is allowed
     */
    m_plan = fftw_plan_dft_r2c_1d( num_samples, m_fft_inp [ DSP_V_CHANNEL ],
                                reinterpret_cast<fftw_complex *>( m_fft_out ),
                                FFTW_ESTIMATE );

    // Start audio reader, providing a callback pointer
    ret = m_reader->start( this );
    if (AAPI_FAILED( ret )) {
        // Failed to start audio reader.
        m_reader->close();
        releaseBuffers();
        return ret;
    }

    return AAPI_SUCCESS;
}

void AAPiSignalProcessor::stop()
{
    m_reader->close();

    releaseBuffers();
}

uint32_t AAPiSignalProcessor::getBufferSize() const
{
    return m_reader->get_buffer_size_chan();
}

void AAPiSignalProcessor::addCallback(AAPiSignalProcessEvents *cb)
{
    m_callbacks.append(cb);
}

} //namespace aapi
