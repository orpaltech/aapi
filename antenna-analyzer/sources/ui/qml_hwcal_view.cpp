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

#include "qml_hwcal_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlHWCalView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlHWCalView::QAAPIQmlHWCalView(AAPIConfig *config, AAPISignalProcessor *processor,
                                     AAPIGenerator *generator, AAPICalibrator *calibrator,
                                     QObject *parent)
    : QAAPIQmlView(config, processor, generator, parent)
{
    m_calibrator = calibrator;

    /* Subscribe for DSP events */
    processor->add_callback( this );
}

QAAPIQmlHWCalView::~QAAPIQmlHWCalView()
{
}

int QAAPIQmlHWCalView::load_view()
{
    return 0;
}

void QAAPIQmlHWCalView::destroy_view()
{
}

int QAAPIQmlHWCalView::on_measure_finished(aapi_measure *measure)
{
    float mag_ratio, phas_diff;
    int ret;

    /* We are in the main thread now */
    if ( measure == nullptr )
    {
        /* Finalize measurement */
        m_calibrator->scan_hwerr_ended();

        /* Save calibration data to file */
        ret = m_calibrator->save_hwerr_file();
    }
    else
    {
        if ( m_scanCancelled ) /*User cancelled*/
        {
            return 1;
        }

        if ( measure->is_signal_low() )
        {
            /* Hardware problem */
            emit scanNoSignal();
            return 1;
        }

        mag_ratio = measure->mag_ratio;
        phas_diff = measure->phas_diff;

        ret = m_calibrator->set_hwerr_entry( m_scanIndex++, mag_ratio, phas_diff);
        /* Notify UI progress */
        emit scanProgress( m_scanIndex, m_allMeasures.length(), mag_ratio, phas_diff);
    }

    return 0;
}

int QAAPIQmlHWCalView::start_hwcal()
{
    aapi_measure_list steps;
    unsigned int freq, num_scans;
    int ret;

    /* Read number of scans*/
    num_scans = m_config->get_osl_n_scans();

    /* Prepare measurement steps */
    for( uint i = 0; i < CALIB_NUM_ENTRIES; i++ )
    {
        freq = AAPICalibrator::freq_by_index(i);

        aapi_ptr<aapi_measure> ptr( aapi_measure::create(m_config, m_calibrator, this,
                                    freq, false, false, num_scans, false) );
        steps.push_back(ptr);
    }

    m_calibrator->scan_hwerr_begin();

    /* reset scan index */
    m_scanIndex = 0;
    m_scanCancelled = false;

    ret = start_measure(steps);
    if (AAPI_FAILED(ret))
    {
        return ret;
    }

    return 0;
}

void QAAPIQmlHWCalView::cancel_hwcal()
{
    m_scanCancelled = true;
}
