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
                                        bool use_hw_err_corr, bool use_osl_corr,
                                        uint32_t max_scans, bool addRef)
{
    AAPiMeasureTask *obj = create(addRef);
    if( obj ) {
        obj->config         = config;
        obj->calibrator     = calibrator;
        obj->callback       = callback;

        obj->measure_freq   = measure_freq;
        obj->max_scans      = max_scans;
        obj->scan_count     = (int) max_scans;
        obj->use_hw_err_corr    = use_hw_err_corr;
        obj->use_osl_corr   = use_osl_corr;
        obj->num_retries    = 3;

        // allocate internal buffers
        obj->mag_v_buf      = static_cast<AAPiReal *> (malloc( max_scans * sizeof(AAPiReal) ));
        obj->mag_i_buf      = static_cast<AAPiReal *> (malloc( max_scans * sizeof(AAPiReal) ));
        obj->phase_diff_buf = static_cast<AAPiReal *> (malloc( max_scans * sizeof(AAPiReal) ));

        // read configuration 
        obj->R0             = config->get_base_r0();
        obj->Rx_raw         = AAPiComplex( obj->R0, 0. );
        obj->Rx             = obj->Rx_raw;
        obj->R_measure      = config->get_bridge_r_measure();
        obj->R_measure_add  = config->get_bridge_r_measure_add();
        obj->R_load         = config->get_bridge_r_load();
        obj->R_total        = ( obj->R_measure + obj->R_measure_add + obj->R_load );

        // set default parameters 
        obj->mag_mv_v       = 1.;
        obj->mag_mv_i       = 1.;
        obj->mag_ratio      = 1.;
        obj->mag_ratio_db   = 0.;
        obj->phase_diff     = 0.;
        obj->phase_diff_d   = 0.;
        obj->component_val  = 0;
    }

    return obj;
}

AAPiMeasureTask::AAPiMeasureTask()
    : calibrator( nullptr )
    , callback( nullptr )
    , mag_v_buf( nullptr )
    , mag_i_buf( nullptr )
    , phase_diff_buf( nullptr )
{
}

AAPiMeasureTask::~AAPiMeasureTask()
{
    free( mag_v_buf );
    free( mag_i_buf );
    free( phase_diff_buf );
}

AAPiComplex AAPiMeasureTask::calc_rx()
{
    AAPiReal r, x;

    r = ( std::cos( phase_diff ) * R_total * mag_ratio ) - ( R_measure + R_measure_add );

    /* Real part may be negative here, OSL calibration gets rid of the sign */
    x = std::sin( phase_diff ) * R_total * mag_ratio;

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

AAPiReal AAPiMeasureTask::calc_vswr(const AAPiComplex& z, AAPiReal r0)
{
    AAPiReal r, swr, ro, x2;

    r = z.real();
    x2 = MathUtils::sqr( z.imag());

    if( r < 0. )
        r = 0.;

    ro = std::sqrt(
                ( MathUtils::sqr( r - r0 ) + x2 )
                / MathUtils::_nonz( MathUtils::sqr( r + r0 ) + x2 )
                );

    if( ro > 0.999f )
        ro = 0.999f;

    swr = ( 1. + ro ) / ( 1. - ro );

    return swr;
}

AAPiReal AAPiMeasureTask::calc_phase_diff(AAPiReal im_i, AAPiReal im_v)
{
    // calculate diff
    AAPiReal diff = im_i - im_v;

    // correct phase difference quadrant
    diff = std::fmod( diff + math::pi, math::pi*2 ) - math::pi;

    if( diff < -math::pi )
        diff += math::pi*2;

    else if( diff > math::pi )
        diff -= math::pi*2;

    return diff;
}

AAPiReal AAPiMeasureTask::calc_return_loss(AAPiReal vswr)
{
    // Handle invalid uninitialized variables or impossible negative VSWR inputs
    if (vswr < 1.0 || std::isnan(vswr) || std::isinf(vswr)) {
        return 0.0; // Terrible match / error state
    }

    // Perfect match ceiling limit
    if (vswr <= 1.0001) {
        return 99.9;
    }

    // High boundary limit (e.g., open/short circuit port state)
    // If VSWR is greater than 100, the return loss is virtually 0 dB anyway
    if (vswr > 100.0) {
        return 0.0;
    }

    // Calculate reflection coefficient magnitude (rho) from VSWR
    AAPiReal rho = (vswr - 1.0) / (vswr + 1.0);

    // Return Loss = -20 * log10(rho)
    AAPiReal return_loss_db = -20.0 * std::log10(rho);

    return return_loss_db;
}

/*
 * Filter array of doubles of length n to remove outliers and return
 * mean of remaining entries that fall into 1 sigma interval.
 * In normal distribution i.e. our case ~68% entries fall into single
 * standard deviation range.
 */
AAPiReal AAPiMeasureTask::process_array(AAPiReal *arr, uint32_t len, int retries)
{
    if( len == 0 )
        return 0.;

    // calculate mean value
    AAPiReal mean = 0.;
    for ( uint i = 0; i < len; ++i ) {
        mean += arr[i];
    }
    mean /= len;

    if ( len < 5 ) /* simple case, return mean */
        return mean;

    // Filter outliers

    // Calculate standard deviation (sigma)
    AAPiReal deviation = 0.;
    for ( uint i = 0; i < len; ++i ) {
        AAPiReal t = arr[i] - mean;
        deviation += (t * t);
    }
    deviation = std::sqrt( deviation / len );

    // calculate mean of entries within part of standard deviation range
    AAPiReal bot = mean - deviation * 0.75;
    AAPiReal top = mean + deviation * 0.75;

    uint count = 0;
    AAPiReal ret = 0.;
    for ( uint i = 0; i < len; i++ ) {
        if ( arr[i] >= bot && arr[i] <= top ) {
            ret += arr[i];
            count++;
        }
    }

    if ( retries && count < len/2 ) {
        return 0.;
    }

    if ( count == 0 ) {
        // nothing falls into range, simply reaturn mean
        return mean;
    }

    ret /= count;
    return ret;
}

void AAPiMeasureTask::process_mags(const AAPiComplex& mag_v, const AAPiComplex& mag_i)
{
    if (scan_count > 0) {

        if ( ! AAPiConfig::is_freq_in_band( measure_freq )) {
            // use default results for out-of-band measure 
            mag_mv_v = 500.;
            mag_mv_i = 500.;
            scan_count = 0;

            // measure complete, issue callback
            if ( callback ) {
                callback->onMeasureTaskFinished( this );
            }

            return;
        }

        int index = scan_count - 1;
        // store V-channel real part
        mag_v_buf[index] = mag_v.real();

        // store I-channel real part
        mag_i_buf[index] = mag_i.real();

        // calculate phase diff
        phase_diff_buf[index] = calc_phase_diff( mag_i.imag(), mag_v.imag() );

        // check if this is the last scan
        if (--scan_count == 0) {

            if ( calc_finalize( )) {
                // measurements complete, issue callback
                if ( callback ) {
                    callback->onMeasureTaskFinished( this );
                }

            } else {
                // need to measure again - too much noise detected
            }
        }
    }
}

bool AAPiMeasureTask::is_low_signal() const
{
    // Comparing with 10.0 mV now is completely valid if you want to detect
    // a weak or heavily attenuated signal before it hits the noise floor!
    constexpr AAPiReal SIGNAL_WEAK_LIMIT_MV = 10.0;

    if (mag_mv_v < SIGNAL_WEAK_LIMIT_MV || mag_mv_i < SIGNAL_WEAK_LIMIT_MV) {
        /* hardware problem ? */
        return true;
    }

    return false;
}

bool AAPiMeasureTask::calc_finalize()
{
    // finalize measurement
    AAPiReal mag_v  = process_array( mag_v_buf, max_scans, num_retries );
    AAPiReal mag_i  = process_array( mag_i_buf, max_scans, num_retries );

    phase_diff = process_array( phase_diff_buf, max_scans, num_retries );

    bool bad_values = (mag_v == 0. || mag_i == 0. || phase_diff == 0.);
    if (num_retries > 0 && bad_values) {
        // Need to measure again - too much noise detected
        scan_count = (int) max_scans;
        num_retries--;
        return false;
    }

    // Scale up to true physical millivolts
    mag_mv_v = mag_v * AAPI_MAG_CORR_FACTOR(config->get_dsp_digital_preamp());
    mag_mv_i = mag_i * AAPI_MAG_CORR_FACTOR(config->get_dsp_digital_preamp());

    if (bad_values) {

        // Force mag_ratio to an ultra-low value (-120 dB)
        mag_ratio = 1e-6;
        mag_ratio_db = -120.0;

        // Force a safe, standard phase alignment (0 degrees / 0 radians)
        phase_diff = 0.0;
        phase_diff_d = 0.0;

        // Force a clean, standard baseline complex impedance.
        // We set it to system reference (R0), which is typically 50.0 + j0 Ohms.
        Rx_raw = AAPiComplex(R0, 0.0);
        Rx = Rx_raw;

        // A perfect 50 Ohm match corresponds to a VSWR = 1.0
        vswr = 1.0;

    } else {

        mag_ratio = mag_v / MathUtils::_nonz( mag_i );

        if ( use_hw_err_corr && calibrator ) {
            // Perform hardware error correction
            calibrator->correct_hw_err( measure_freq, mag_ratio, phase_diff );
        }

        mag_ratio_db = 20. * std::log10( mag_ratio );

        phase_diff_d = ( phase_diff * 180. ) / math::pi;

        // calculate complex impedance
        Rx_raw = calc_rx( );
        Rx = Rx_raw;

        if ( use_osl_corr && calibrator ) {
            // Perform OSL error correction
            calibrator->correct_z( measure_freq, Rx );
        }

        // calculate VSWR
        vswr = calc_vswr( Rx, R0 );
    }

    return_loss_db = calc_return_loss(vswr);
    calc_component();

    return true;
}

void AAPiMeasureTask::calc_component()
{
    AAPiReal reactance = Rx.imag();

    // Calculate angular frequency (omega = 2 * pi * f)
    // Ensure measure_freq is strictly in Hz!
    AAPiReal omega = 2.0 * math::pi * measure_freq;

    // Enhanced Safety Check for noise/zero-crossings
    if (omega <= 0.0 || std::abs(reactance) < 1e-4) { // Slightly wider threshold for real VNA noise
        is_inductive = false; // Prevents UI symbol flickering
        component_val = 0.0;
        return;
    }

    // Determine component type based on the sign of the Reactance (X)
    is_inductive = (reactance > 0.0);

    // Calculate the component value
    if (is_inductive) {
        // L = X / omega (Result is in Henrys)
        component_val = reactance / omega;
    } else {
        // C = 1 / (omega * |X|) (Result is in Farads)
        component_val = 1.0 / (omega * std::abs(reactance));
    }
}

} //namespace aapi
