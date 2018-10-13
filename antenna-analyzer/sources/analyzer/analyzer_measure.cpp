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

#include "analyzer_measure.h"
#include "analyzer/math_utils.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// aapi_measure
///////////////////////////////////////////////////////////////////////////////

aapi_measure *aapi_measure::create(AAPIConfig *config, AAPICalibrator *calibrator,
                                   AAPIMeasurementEvents *callback,
                                   uint32_t freq,
                                   bool correct_hwerr,
                                   bool correct_osl,
                                   uint32_t num_measures,
                                   bool add_ref)
{
    aapi_measure *measure = create(add_ref);
    if ( measure )
    {
        measure->m_calibrator = calibrator;

        measure->m_callback = callback;
        measure->freq = freq;
        measure->m_numMeasures = num_measures;
        measure->m_iter = measure->m_numMeasures;
        measure->m_hwErrCorrection = correct_hwerr;
        measure->m_oslCorrection = correct_osl;
        measure->m_retries = 3;

        /* allocate internal buffers */
        measure->mag_v_buf = reinterpret_cast<float*> (malloc(num_measures));
        measure->mag_i_buf = reinterpret_cast<float*> (malloc(num_measures));
        measure->phas_diff_buf = reinterpret_cast<float*> (malloc(num_measures));

        /* read configuration */
        measure->r0     = config->get_base_r0();
        measure->rx     = std::complex<float>(measure->r0, 0.f);
        measure->r_meas = config->get_bridge_r_meas();
        measure->r_meas_add = config->get_bridge_r_meas_add();
        measure->r_load     = config->get_bridge_r_load();
        measure->r_total    = measure->r_meas +
                                measure->r_meas_add +
                                measure->r_load;

        /* set default parameters */
        measure->mag_mv_v   = 1.f;
        measure->mag_mv_i   = 1.f;
        measure->mag_ratio      = 1.f;
        measure->mag_ratio_db   = 0.f;
        measure->phas_diff      = 0.f;
        measure->phas_diff_d    = 0.f;
    }

    return measure;
}

aapi_measure::aapi_measure()
    : m_callback( nullptr )
    , mag_v_buf( nullptr )
    , mag_i_buf( nullptr )
    , phas_diff_buf( nullptr )
{
}

aapi_measure::~aapi_measure()
{
    free ( mag_v_buf );
    free ( mag_i_buf );
    free ( phas_diff_buf );
}

std::complex<float> aapi_measure::calc_rx()
{
    float r, x;

    r = ( std::cos( phas_diff ) * r_total * mag_ratio ) - ( r_meas + r_meas_add );
    /* Real part may be negative here, OSL calibration gets rid of the sign */
    x = std::sin( phas_diff ) * r_total * mag_ratio;

    std::complex<float> rx( r, x );
    /* Result validation */
    if ( std::isnan( rx.real() ) || std::isinf( rx.real() ) )
    {
        rx = std::complex<float>( 0.f, rx.imag() );
    }
    if ( std::isnan( rx.imag() ) || std::isinf( rx.imag() ) )
    {
        rx = std::complex<float>( rx.real(), 0.f );
    }

    return rx;
}

float aapi_measure::calc_vswr(const std::complex<float>& z, float r0)
{
    float r, swr, ro, x2;

    r = z.real();
    x2 = std::pow( z.imag(), 2 );

    if ( r < 0.f )
        r = 0.f;

    ro = std::sqrt(
                ( std::pow(r - r0, 2) + x2 )
                / AAPIMathUtils::_nonz( std::pow(r + r0, 2) + x2 )
                );

    if ( ro > 0.999f )
        ro = 0.999f;

    swr = ( 1.f + ro ) / ( 1.f - ro );
    return swr;
}

float aapi_measure::calc_phase_diff(float im_i, float im_v)
{
    // calculate diff
    float diff = im_i - im_v;

    // correct phase difference quadrant
    diff = std::fmod( diff + M_PI, M_PI*2 ) - M_PI;

    if( diff < -M_PI )
        diff += M_PI*2;
    else if( diff > M_PI )
        diff -= M_PI*2;

    return diff;
}

/*
 * Filter array of floats of length n to remove outliers and return
 * mean of remaining entries that fall into 1 sigma interval.
 * In normal distribution i.e. our case ~68% entries fall into single
 * standard deviation range.
 */
float aapi_measure::filter_array(float *arr, int len, int retries)
{
    float mean = 0.f;
    float deviation = 0.f;
    float bot, top;
    float ret = 0.f;
    int count = 0, i;

    if (len <= 0)
        return 0.f;

    /* calculate mean value */
    for ( i = 0; i < len; i++ )
    {
        mean += arr[i];
    }
    mean /= len;

    if ( len < 5 ) /* simple case, return mean */
        return mean;

    /* Filter outliers */

    /* calculate standard deviation (sigma) */
    for ( i = 0; i < len; i++ )
    {
        float t = arr[i] - mean;
        deviation += (t * t);
    }
    deviation = std::sqrt(deviation / len);

    /* calculate mean of entries within part of standard deviation range */
    bot = mean - deviation * 0.75f;
    top = mean + deviation * 0.75f;

    for ( i = 0; i < len; i++ )
    {
        if (arr[i] >= bot && arr[i] <= top)
        {
            ret += arr[i];
            count++;
        }
    }

    if ( retries && count < len/2 )
        return 0.f;

    if ( count == 0 ) /* nothing falls into range, simply reaturn mean */
        return mean;

    ret /= count;
    return ret;
}

int aapi_measure::process_mags(const std::complex<float>& v_mag, const std::complex<float>& i_mag)
{
    if ( m_iter > 0 )
    {
        if (! AA_IS_FREQ_INBAND( freq ))
        {
            /* use default results for out-of-band measure */
            mag_mv_v = 500.f;
            mag_mv_i = 500.f;
            m_iter = 0;

            /* measure complete, issue callback */
            if (m_callback)
                m_callback->measure_finished(this);

            return 1;
        }

        int i = m_iter - 1;
        // store V-channel real part
        mag_v_buf[i] = v_mag.real();

        // store I-channel real part
        mag_i_buf[i] = i_mag.real();

        // calculate phase diff
        phas_diff_buf[i] = calc_phase_diff(i_mag.imag(), v_mag.imag());

        /* decrement measurement iterator */
        if ( --m_iter == 0 )
        {
            if (! calc_finalize())
            {
                /* need to measure again - too much noise detected */
            }
            else
            {
                /* measurements complete, issue callback */
                if (m_callback)
                    m_callback->measure_finished(this);
                return 1;
            }
        }
    }

    return 0;
}

bool aapi_measure::is_signal_low() const
{
    if ( mag_mv_i < 10.f || mag_mv_v < 10.f )
    {
        /* hardware problem */
        return true;
    }
    return false;
}

bool aapi_measure::calc_finalize()
{
    float mag_v, mag_i;

    /* finalize measurement */
    mag_v   = filter_array( mag_v_buf, m_numMeasures, m_retries );
    mag_i   = filter_array( mag_i_buf, m_numMeasures, m_retries );
    phas_diff = filter_array( phas_diff_buf, m_numMeasures, m_retries );

    if ( mag_v == 0.f || mag_i == 0.f || phas_diff == 0.f )
    {
        /* Need to measure again - too much noise detected */
        m_retries --;
        m_iter = m_numMeasures;
        return false;
    }

    mag_ratio = mag_v / AAPIMathUtils::_nonz(mag_i);

    if ( m_hwErrCorrection )
    {
        /* do hardware error correction */
        m_calibrator->correct_hwerr( freq, &mag_ratio, &phas_diff );
    }

    mag_ratio_db = 20.f * std::log10(mag_ratio);
    phas_diff_d = ( phas_diff * 180.f ) / M_PI;

    // calculate derived results
    mag_mv_v = mag_v * AA_MAG_CORR_FACTOR;
    mag_mv_i = mag_i * AA_MAG_CORR_FACTOR;

    /* complex impedance */
    this->rx = calc_rx();

    if ( m_oslCorrection )
    {
        /* do OSL error correction */
        m_calibrator->correct_z(freq, this->rx);
    }

    /* calculate VSWR */
    this->vswr = calc_vswr(this->rx, this->r0);

    return true;
}

} //namespace aapi
