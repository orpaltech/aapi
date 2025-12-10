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

#include "aapi_measure_task.h"
#include "utils/aapi_math_utils.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPiMeasureTask
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT(AAPiMeasureTask)

AAPiMeasureTask *AAPiMeasureTask::create(AAPiConfig *config, AAPiCalibrator *calibrator,
                                        AAPiMeasureEvents *callback, uint32_t measure_freq,
                                        bool correct_hwerr, bool correct_osl,
                                        uint32_t num_measures, bool addRef)
{
    AAPiMeasureTask *obj = create(addRef);
    if( obj ) {
        obj->calibrator     = calibrator;
        obj->callback       = callback;

        AAPI_ADDREF(calibrator);

        obj->measure_freq   = measure_freq;
        obj->num_measures   = num_measures;
        obj->measure_iter   = static_cast<int> ( num_measures );
        obj->correct_hwerr  = correct_hwerr;
        obj->correct_osl    = correct_osl;
        obj->num_retries    = 3;

        // allocate internal buffers
        obj->mag_v_buf    = reinterpret_cast<double *> (malloc( num_measures * sizeof(double) ));
        obj->mag_i_buf    = reinterpret_cast<double *> (malloc( num_measures * sizeof(double) ));
        obj->phas_diff_buf = reinterpret_cast<double *> (malloc( num_measures * sizeof(double) ));

        // read configuration 
        obj->R0             = config->get_base_r0();
        obj->Rx             = AAPiComplex( obj->R0, 0. );
        obj->R_measure      = config->get_bridge_r_measure();
        obj->R_measure_add  = config->get_bridge_r_measure_add();
        obj->R_load         = config->get_bridge_r_load();
        obj->R_total        = ( obj->R_measure + obj->R_measure_add + obj->R_load );

        // set default parameters 
        obj->mag_mv_v       = 1.;
        obj->mag_mv_i       = 1.;
        obj->mag_ratio      = 1.;
        obj->mag_ratio_db   = 0.;
        obj->Phas_diff      = 0.;
        obj->Phas_diff_d    = 0.;
    }

    return obj;
}

AAPiMeasureTask::AAPiMeasureTask()
    : calibrator(nullptr)
    , callback( nullptr )
    , mag_v_buf( nullptr )
    , mag_i_buf( nullptr )
    , phas_diff_buf( nullptr )
{
}

AAPiMeasureTask::~AAPiMeasureTask()
{
    free( mag_v_buf );
    free( mag_i_buf );
    free( phas_diff_buf );

    AAPI_DISPOSE( calibrator );
}

AAPiComplex AAPiMeasureTask::calc_rx()
{
    double r, x;

    r = ( std::cos( Phas_diff ) * R_total * mag_ratio ) - ( R_measure + R_measure_add );

    /* Real part may be negative here, OSL calibration gets rid of the sign */
    x = std::sin( Phas_diff ) * R_total * mag_ratio;

    AAPiComplex rx( r, x );

    /* Result validation */
    if (std::isnan( rx.real() ) || std::isinf( rx.real() ))
    {
        rx = AAPiComplex( 0., rx.imag() );
    }
    if (std::isnan( rx.imag() ) || std::isinf( rx.imag() ))
    {
        rx = AAPiComplex( rx.real(), 0. );
    }

    return rx;
}

double AAPiMeasureTask::calc_vswr(const AAPiComplex& z, double r0)
{
    double r, swr, ro, x2;

    r = z.real();
    x2 = std::pow( z.imag(), 2 );

    if( r < 0. )
        r = 0.;

    ro = std::sqrt(
                ( std::pow(r - r0, 2) + x2 )
                / MathUtils::_nonz( std::pow(r + r0, 2) + x2 )
                );

    if( ro > 0.999f )
        ro = 0.999f;

    swr = ( 1. + ro ) / ( 1. - ro );

    return swr;
}

double AAPiMeasureTask::calc_phase_diff(double im_i, double im_v)
{
    // calculate diff
    double diff = im_i - im_v;

    // correct phase difference quadrant
    diff = std::fmod( diff + M_PI, M_PI*2 ) - M_PI;

    if( diff < -M_PI )
        diff += M_PI*2;

    else if( diff > M_PI )
        diff -= M_PI*2;

    return diff;
}

/*
 * Filter array of doubles of length n to remove outliers and return
 * mean of remaining entries that fall into 1 sigma interval.
 * In normal distribution i.e. our case ~68% entries fall into single
 * standard deviation range.
 */
double AAPiMeasureTask::filter_array(double *arr, uint32_t len, int retries)
{
    double mean = 0.;
    double deviation = 0.;
    double bot, top;
    double ret = 0.;
    int count = 0;
    unsigned i;

    if( len <= 0 )
        return 0.;

    // calculate mean value
    for (i = 0; i < len; i++)
    {
        mean += arr[i];
    }
    mean /= len;

    if ( len < 5 ) /* simple case, return mean */
        return mean;

    /* Filter outliers */

    // calculate standard deviation (sigma)
    for ( i = 0; i < len; i++ )
    {
        double t = arr[i] - mean;
        deviation += (t * t);
    }
    deviation = std::sqrt( deviation / len );

    // calculate mean of entries within part of standard deviation range
    bot = mean - deviation * 0.75;
    top = mean + deviation * 0.75;

    for ( i = 0; i < len; i++ )
    {
        if ( arr[i] >= bot && arr[i] <= top )
        {
            ret += arr[i];
            count++;
        }
    }

    if ( retries && count < len/2 )
    {
        return 0.;
    }

    if ( count == 0 )
    {
        // nothing falls into range, simply reaturn mean
        return mean;
    }

    ret /= count;
    return ret;
}

bool AAPiMeasureTask::process_mags(const AAPiComplex& v_mag, const AAPiComplex& i_mag)
{
    if ( measure_iter > 0 )
    {
        if ( ! AAPI_IS_FREQ_INBAND( this->measure_freq ))
        {
            // use default results for out-of-band measure 
            mag_mv_v = 500.;
            mag_mv_i = 500.;
            measure_iter = 0;

            // measure complete, issue callback
            if ( callback )
            {
                callback->onMeasureFinished( this );
            }

            return true;
        }

        int i = measure_iter - 1;
        // store V-channel real part
        this->mag_v_buf[i] = v_mag.real();

        // store I-channel real part
        this->mag_i_buf[i] = i_mag.real();

        // calculate phase diff
        this->phas_diff_buf[i] = calc_phase_diff( i_mag.imag(), v_mag.imag() );

        // decrement measurement iterator
        if ( --measure_iter == 0 )
        {
            if ( calc_finalize( ))
            {
                // measurements complete, issue callback
                if ( callback )
                {
                    callback->onMeasureFinished( this );
                }

                return true;
            }
            else
            {
                /* need to measure again - too much noise detected */
            }
        }
    }

    return false;
}

bool AAPiMeasureTask::is_low_signal() const
{
    if ( mag_mv_i < 10. || mag_mv_v < 10. )
    {
        /* hardware problem ? */
        return true;
    }
    return false;
}

bool AAPiMeasureTask::calc_finalize()
{
    double mag_v, mag_i;

    // finalize measurement
    mag_v   = filter_array( mag_v_buf, num_measures, num_retries );
    mag_i   = filter_array( mag_i_buf, num_measures, num_retries );

    Phas_diff = filter_array( phas_diff_buf, num_measures, num_retries );

    if( mag_v == 0. || mag_i == 0. || Phas_diff == 0. )
    {
        // Need to measure again - too much noise detected 
        num_retries --;
        measure_iter = (int) num_measures;
        return false;
    }

    mag_ratio = mag_v / MathUtils::_nonz( mag_i );

    if( correct_hwerr && calibrator )
    {
        // do hardware error correction
        calibrator->correct_hwe( measure_freq, &mag_ratio, &Phas_diff );
    }

    mag_ratio_db = 20. * std::log10( mag_ratio );
    Phas_diff_d = ( Phas_diff * 180. ) / M_PI;

    // calculate derived results
    mag_mv_v = mag_v * AAPI_MAG_CORR_FACTOR;
    mag_mv_i = mag_i * AAPI_MAG_CORR_FACTOR;

    // calculate complex impedance 
    Rx = calc_rx( );

    if( correct_osl && calibrator )
    {
        // do OSL error correction 
        calibrator->correct_z( measure_freq, Rx );
    }

    // calculate VSWR 
    vswr = calc_vswr( Rx, R0 );

    return true;
}

} //namespace aapi
