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

#ifndef AAPI_MEASURE_TASK_H
#define AAPI_MEASURE_TASK_H

#include "analyzer/aapi_config.h"
#include "analyzer/aapi_calibrator.h"

namespace aapi
{

class AAPiMeasureTask;

///////////////////////////////////////////////////////////////////////////////
// class AAPiMeasureEvents
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiMeasureEvents class
///
class AAPiMeasureEvents
{
protected:
    AAPiMeasureEvents() {}

public:
    virtual ~AAPiMeasureEvents() {}

    virtual void onMeasureTaskFinished(AAPiMeasureTask *measure) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiMeasureTask
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiMeasureTask class
///
class AAPiMeasureTask : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AAPiMeasureTask)

    static AAPiMeasureTask *create(AAPiConfig *config, AAPiCalibrator *calibrator,
                                    AAPiMeasureEvents *callback, uint32_t measure_freq,
                                    bool use_hw_err_corr, bool use_osl_corr,
                                    uint32_t num_scans, bool addRef = true);
protected:
    AAPiMeasureTask();
    ~AAPiMeasureTask();

public:
    void process_mags(const AAPiComplex& mag_v, const AAPiComplex& mag_i);
    bool is_low_signal() const;

private:
    bool calc_finalize();

    AAPiComplex calc_rx();
    void calc_component();

    static AAPiReal calc_vswr(const AAPiComplex& z, AAPiReal r0);
    static AAPiReal calc_phase_diff(AAPiReal im_i, AAPiReal im_v) ;
    static AAPiReal calc_return_loss(AAPiReal vswr);

    /* Filter array of doubles of length n to remove outliers and return
     * the mean of remaining entries that fall into 1 sigma interval.
     * In normal distribution i.e. our case ~68% entries fall into single
     * standard deviation range. */
    static AAPiReal process_array(AAPiReal *arr, uint32_t len, int retries);

private:
    AAPiPtr<AAPiConfig>     config;
    AAPiPtr<AAPiCalibrator> calibrator;
    AAPiMeasureEvents       *callback;

    AAPiReal    *mag_v_buf;
    AAPiReal    *mag_i_buf;
    AAPiReal    *phase_diff_buf;

    int         num_retries;
    int         scan_count;
    uint32_t    max_scans;

    bool        use_osl_corr;
    bool        use_hw_err_corr;

    AAPiReal    R0;             /* System impedance (real value, i.e. 50 Ohm)*/
    AAPiReal    R_measure;      /* Bridge measurement resistor */
    AAPiReal    R_measure_add;  /* Bridge add resistor */
    AAPiReal    R_load;         /* Bridge load resistor */
    AAPiReal    R_total;

public:
    uint32_t    measure_freq;

    AAPiReal    mag_mv_v;       /* Measured magnitude in mV for V-channel */
    AAPiReal    mag_mv_i;       /* Measured magnitude in mV for I-channel */
    AAPiReal    mag_ratio;      /* Measured magnitude ratio */
    AAPiReal    mag_ratio_db;   /* Measured magnitude ratio, dB */
    AAPiReal    phase_diff;     /* Measured phase difference in radians */
    AAPiReal    phase_diff_d;   /* Measured phase difference in degrees */
    AAPiComplex Rx;             /* Measured complex resistance (OSL-corrected) */
    AAPiComplex Rx_raw;         /* Measured complex resistance (raw) */
    AAPiReal    vswr;           /* Measured VSWR */

    /* Return loss (expressed in decibels, dB) measures how much power is reflected back
     * toward the source due to an impedance mismatch.
     * A higher return loss is better, as it means less power is wasted via reflections.
     */
    AAPiReal    return_loss_db;

    bool        is_inductive;
    AAPiReal    component_val;
};

} // namespace aapi

#endif // AAPI_MEASURE_TASK_H
