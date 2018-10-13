/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#include <math.h>
#include <fftw3.h>
#include <string.h>
#include "analyzer_dsp.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPISignalProcessor
///////////////////////////////////////////////////////////////////////////////

AAPISignalProcessor *AAPISignalProcessor::create(AAPIConfig *config, bool add_ref)
{
    AAPISignalProcessor *dsp = create(add_ref);
    if( dsp )
        dsp->m_config = config;
    return dsp;
}

AAPISignalProcessor::AAPISignalProcessor()
    : m_reader(AAPIAudioReader::create(false))
    , fft_out(nullptr)
    , window(nullptr)
    , plan(nullptr)
{
    fft_inp[0] = nullptr;
    fft_inp[1] = nullptr;
    fft_mags[0] = nullptr;
    fft_mags[1] = nullptr;
}

AAPISignalProcessor::~AAPISignalProcessor()
{
    stop();
}

void AAPISignalProcessor::free_buffers()
{
    fftwf_free( fft_inp[0] );
    fftwf_free( fft_inp[1] );
    fftwf_free( fft_out );
    fftwf_free( window );
    fftwf_free( fft_mags[0] );
    fftwf_free( fft_mags[1] );

    fft_inp[0] = nullptr;
    fft_inp[1] = nullptr;

    fft_out = nullptr;
    window = nullptr;

    fft_mags[0] = nullptr;
    fft_mags[1] = nullptr;
}

float AAPISignalProcessor::mag2db(float mag)
{
    return 20.f * std::log10( mag );
}

void AAPISignalProcessor::blackman(float *wnd, uint32_t num)
{
    for( uint i = 0; i < num; i++ )
    {
        wnd[ i ] = static_cast<float> (
                0.426591 -
                0.496561 * std::cos( ( 2.0 * M_PI * i ) / ( num - 1 ) ) +
                0.076848 * std::cos( ( 4.0 * M_PI * i ) / ( num - 1 ) )
        );
    }
}

std::complex<float> AAPISignalProcessor::calc_magnitude(int channel)
{
    std::complex< float > *bf;
    fftwf_complex *out;
    float mag, phase, pwr;
    uint32_t bin, i;

    out = reinterpret_cast<fftwf_complex *>( fft_out );
    bin = m_config->get_dsp_fftbin();
    pwr = 0.f;

    /* calculate signal magnitude for a bin considering
     * +/- 2 bins from maximum */

    for( i = bin - 2; i <= bin + 2; i++ )
    {
        mag = fft_mags[channel][i];
        pwr += std::pow( mag, 2 );
    }

    mag = sqrtf( pwr );

    /* according to FFTW3 documenation, one is allowed to do
     * direct cast from fftw_complex to std::complex */
    bf = reinterpret_cast<std::complex< float > *>( &out[bin] );
    phase = std::atan2( bf->imag(), bf->real() );

    return std::complex< float >( mag, phase );
}

uint32_t AAPISignalProcessor::process_buffer(int channel, char *buffer, uint32_t /*buff_size*/)
{
    fftwf_plan plan = nullptr; //(fftwf_plan) this->plan; // TODO: remove
    uint32_t num_samples, num_fft, i;
    std::complex<float> *bf ;
    fftwf_complex *out = reinterpret_cast<fftwf_complex *>(fft_out);
    AAPIAudioSampleSize samp_size
            = static_cast<AAPIAudioSampleSize>( m_config->get_dsp_sample_size() );
    int32_t samp_val;

    /* number of FFT points */
    num_samples = m_config->get_dsp_nsamples();
    num_fft = num_samples / 2;

    /* copy audio samples in the input array */
    for( i = 0; i < num_samples; i++ )
    {
        switch( samp_size )
        {
            case AUDIO_SSIZE_16:
            {
                /* 16bit, signed integer */
                int16_t *samp_buff = reinterpret_cast<int16_t *> (buffer);
                samp_val = samp_buff[i];
                break;
            }
            case AUDIO_SSIZE_24:
            case AUDIO_SSIZE_32:
            {
                /* 32bit, signed integer */
                int32_t *samp_buff = reinterpret_cast<int32_t *> (buffer);
                samp_val = samp_buff[i];
                break;
            }
        }

        fft_inp [channel][i] = ( window[i] * samp_val );
    }

    // TODO: optimization attempt
    // can we create a single plan for multiple frames ?
    //if ( plan == nullptr ) {

    /* Create a new FFTW plan for "real-to-complex" 1-dim transform */
    plan = fftwf_plan_dft_r2c_1d( num_samples, fft_inp [ channel ],
                                  out, FFTW_ESTIMATE );

        /*Store calculated plan*/
        //this->plan = plan;
    //}

    /* execute FFT */
    fftwf_execute( plan );

    /* release memory, associated with FFTW plan */
    fftwf_destroy_plan( plan );

    for( i = 0; i < num_fft; i++ )
    {
        /* according to FFTW3 documenation, one is allowed to do
         * direct cast from fftw_complex to std::complex */
        bf = reinterpret_cast< std::complex<float> *>( &out[i] );

        /* normalize output array */
        fft_mags [channel][i] = std::abs( *bf ) / ( num_fft );
    }

    /* calculate signal magnitude for a bin considering
     * +/- 2 bins from maximum */
    fft_xmag [channel] = calc_magnitude( channel );

    return ( num_fft );
}

void AAPISignalProcessor::audio_reader_data(char **buffers, uint32_t num_channels, uint32_t buf_size)
{
    if( get_num_callbacks() == 0 )
        return;

    if( num_channels < 2 )
        return;

    bool enabled = false;
    int i, ch;
    AAPISignalProcessorEvents *cb;
    uint32_t num_inp, num_fft = 0;

    /* number of points */
    num_inp = m_config->get_dsp_nsamples();

    /* check if at least one callback is enabled */
    for( i = 0; i < get_num_callbacks(); i++ )
    {
        if( get_callback(i)->is_signal_processing() )
        {
            enabled = true;
            break;
        }
    }

    if( !enabled )
        return;

    for( ch = 0; ch < num_channels; ch++ )
    {
        num_fft = process_buffer( ch, buffers[ch], buf_size );
    }

    for( i = 0; i < get_num_callbacks(); i++ )
    {
        cb = get_callback(i);
        if( cb && cb->is_signal_processing() )
        {
            /* run callbacks */
            cb->dsp_raw_frame( fft_inp, num_channels, num_inp );
            cb->dsp_fft_frame( fft_mags, num_channels, num_fft );
            cb->dsp_magnitudes( fft_xmag, num_channels );
        }
    }
}

int AAPISignalProcessor::start()
{
    /* read configuration parameters for ADC */
    QString dev_name;
    uint32_t num_samples;
    int dev_index = -1;
    int i, ret;
    AAPIAudioSampleRate sample_rate;
    AAPIAudioSampleSize sample_size;

    sample_rate = static_cast< AAPIAudioSampleRate > ( m_config->get_dsp_sample_rate() );
    sample_size = static_cast< AAPIAudioSampleSize > ( m_config->get_dsp_sample_size() );

    dev_name = m_config->get_audio_input_device();
    num_samples = m_config->get_dsp_nsamples();

    for( i = 0; i < m_reader->get_num_devices(); i++ )
    {
        if( dev_name == m_reader->get_device_id(i) )
        {
            if( !m_reader->is_format_supported( i, AUDIO_CHANNELS_2,
                                                sample_rate, sample_size ))
            {
                return AAPIDSP_E_AUDIO_UNSUPPORT_FORMAT;
            }
            dev_index = i;
            break;
        }
    }
    if( dev_index < 0 )
    {
        return AAPIDSP_E_AUDIO_DEVICE_NOT_FOUND;
    }

    /* open audio reader */
    ret = m_reader->open( dev_name.toLatin1(), AUDIO_CHANNELS_2,
                          sample_rate, sample_size, num_samples );
    if( AAPI_FAILED(ret) )
    {
        /* failed to open audio reader. */
        return ret;
    }

    /* allocate required buffers */
    fft_inp[0] = fftwf_alloc_real( num_samples );
    fft_inp[1] = fftwf_alloc_real( num_samples );
    fft_mags[0] = fftwf_alloc_real( num_samples / 2 );
    fft_mags[1] = fftwf_alloc_real( num_samples / 2 );
    fft_out = reinterpret_cast<float *> ( fftwf_alloc_complex (( num_samples / 2 ) + 1));
    window = fftwf_alloc_real( num_samples );

    /* Check if any of memory allocations has failed */
    if (!fft_inp[0] || !fft_inp[1] ||
            !fft_out || !window ||
            !fft_mags[0] || !fft_mags[1]) {
        free_buffers();
        return AAPI_E_OUT_OF_MEMORY;
    }

    /* prepare blackman window */
    blackman( window, num_samples );

    /* Start audio reader, providing a callback pointer */
    ret = m_reader->start( this );
    if( AAPI_FAILED(ret) )
    {
        /* Failed to start audio reader. */
        m_reader->close();
        free_buffers();
        return ret;
    }

    return 0;
}

void AAPISignalProcessor::stop()
{
    m_reader->close();

    free_buffers();
}

} //namespace aapi
