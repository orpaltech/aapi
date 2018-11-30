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

#ifndef AAPI_MEASURE_H
#define AAPI_MEASURE_H

#include <complex>
#include "analyzer/analyzer_config.h"
#include "analyzer/analyzer_calibrator.h"

namespace aapi
{

class AAPIMeasure;

///////////////////////////////////////////////////////////////////////////////
// class AAPIMeasurementEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIMeasurementEvents class
///
class AAPIMeasurementEvents
{
protected:
    AAPIMeasurementEvents() {}
public:
    virtual ~AAPIMeasurementEvents() {}

    virtual void measure_finished(AAPIMeasure *measure) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIMeasure
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIMeasure class
///
class AAPIMeasure :
        public AAPIObject
{
    DECLARE_AAPI_OBJECT(AAPIMeasure)

    static AAPIMeasure *create(AAPIConfig *config, AAPICalibrator *calibrator,
                               AAPIMeasurementEvents *callback,
                               uint32_t frequency,
                               bool correct_hwerr,
                               bool correct_osl,
                               uint32_t num_scans,
                               bool add_ref = true);
private:
    AAPIMeasure();

    std::complex<float> calc_rx();

    static float calc_vswr(const std::complex<float>& z, float r0);
    static float calc_phase_diff(float im_i, float im_v) ;

    /* Filter array of floats of length n to remove outliers and return
     * the mean of remaining entries that fall into 1 sigma interval.
     * In normal distribution i.e. our case ~68% entries fall into single
     * standard deviation range. */
    static float filter_array(float *arr, uint32_t len, int retries);

public:
    ~AAPIMeasure();

    int process_mags(const std::complex<float>& mag_v, const std::complex<float>& mag_i);
    bool calc_finalize();
    bool is_signal_low() const;

private:
    aapi_ptr<AAPICalibrator> calibrator;
    AAPIMeasurementEvents   *callback;

public:
    float               mag_mv_v;       /* Measured magnitude in mV for V-channel */
    float               mag_mv_i;       /* Measured magnitude in mV for I-channel */
    float               mag_ratio;      /* Measured magnitude ratio */
    float               mag_ratio_db;   /* Measured magnitude ratio, dB */
    float               phas_diff;      /* Measured phase difference in radians */
    float               phas_diff_d;    /* Measured phase difference in degrees */
    std::complex<float> mag_v;          /* Measured complex magnitude for V-channel */
    std::complex<float> mag_i;          /* Measured complex magnitude for I-channel */
    std::complex<float> rx;             /* Measured complex resistance */
    float               vswr;           /* Measured VSWR */
    int                 num_retries;
    int                 measure_iter;
    uint32_t            num_measures;
    uint32_t            frequency;
    bool                osl_corr;
    bool                hwerr_corr;
    float               *mag_v_buf;
    float               *mag_i_buf;
    float               *phas_diff_buf;
    float               r0;             /* System impedance (real value, i.e. 50 Ohm)*/
    float               r_measure;      /* Bridge measurement resistor */
    float               r_measure_add;  /* Bridge add resistor */
    float               r_load;         /* Bridge load resistor */
    float               r_total;
};

} // namespace aapi

#endif // AAPI_MEASURE_H
