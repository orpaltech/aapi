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

#ifndef AAPI_MEASURE_TASK_H
#define AAPI_MEASURE_TASK_H

#include <complex>
#include "analyzer/aapi_configuration.h"
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

    virtual void onMeasureFinished(AAPiMeasureTask *measure) = 0;
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
                                    bool correct_hwerr, bool correct_osl,
                                    uint32_t num_scans, bool addRef = true);
protected:
    AAPiMeasureTask();
    ~AAPiMeasureTask();

public:

    bool process_mags(const AAPiComplex& mag_v, const AAPiComplex& mag_i);
    bool calc_finalize();
    bool is_low_signal() const;

private:
    AAPiComplex calc_rx();

    static double calc_vswr(const AAPiComplex& z, double r0);
    static double calc_phase_diff(double im_i, double im_v) ;

    /* Filter array of doubles of length n to remove outliers and return
     * the mean of remaining entries that fall into 1 sigma interval.
     * In normal distribution i.e. our case ~68% entries fall into single
     * standard deviation range. */
    static double filter_array(double *arr, uint32_t len, int retries);

private:
    AAPiCalibrator      *calibrator;
    AAPiMeasureEvents   *callback;

public:
    double      mag_mv_v;       /* Measured magnitude in mV for V-channel */
    double      mag_mv_i;       /* Measured magnitude in mV for I-channel */
    double      mag_ratio;      /* Measured magnitude ratio */
    double      mag_ratio_db;   /* Measured magnitude ratio, dB */
    double      Phas_diff;      /* Measured phase difference in radians */
    double      Phas_diff_d;    /* Measured phase difference in degrees */
    AAPiComplex mag_v;          /* Measured complex magnitude for V-channel */
    AAPiComplex mag_i;          /* Measured complex magnitude for I-channel */
    AAPiComplex Rx;             /* Measured complex resistance */
    double      vswr;           /* Measured VSWR */
    int         num_retries;
    int         measure_iter;
    uint32_t    num_measures;
    uint32_t    measure_freq;
    bool        correct_osl;
    bool        correct_hwerr;
    double      *mag_v_buf;
    double      *mag_i_buf;
    double      *phas_diff_buf;
    double      R0;             /* System impedance (real value, i.e. 50 Ohm)*/
    double      R_measure;      /* Bridge measurement resistor */
    double      R_measure_add;  /* Bridge add resistor */
    double      R_load;         /* Bridge load resistor */
    double      R_total;
};

} // namespace aapi

#endif // AAPI_MEASURE_TASK_H
