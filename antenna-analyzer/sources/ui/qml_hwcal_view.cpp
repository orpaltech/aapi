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

QAAPIQmlHWCalView::QAAPIQmlHWCalView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                     AAPIGenerator *gen, AAPICalibrator *calibrator,
                                     QObject *parent)
    : QAAPIQmlView(config, dsp, gen, parent)
{
    m_calibrator = calibrator;
    AAPI_ADDREF(m_calibrator);

    // Subscribe for DSP events 
    dsp->add_callback( this );
}

QAAPIQmlHWCalView::~QAAPIQmlHWCalView()
{
    AAPI_DISPOSE(m_calibrator);
}

int QAAPIQmlHWCalView::load_view()
{
    return 0;
}

void QAAPIQmlHWCalView::destroy_view()
{
}

int QAAPIQmlHWCalView::on_measure_finished(AAPIMeasure *measure)
{
    float mag_ratio, phas_diff;
    int ret;

    // NOTE: We are in the main thread now 

    if( measure == nullptr )
    {
        // Finalize measurement
        m_calibrator->scan_hwerror_ended();

        // Save calibration data to file 
        ret = m_calibrator->save_hwerror_correction_file();
    }
    else
    {
        if( m_scan_cancelled ) /*User cancelled*/
        {
            return 1;
        }

        if( measure->is_signal_low() )
        {
            // Hardware problem 
            emit scanNoSignal();
            return 1;
        }

        mag_ratio = measure->mag_ratio;
        phas_diff = measure->Phas_diff;

        ret = m_calibrator->set_hwerror_entry( m_scan_index++, mag_ratio, phas_diff);
        
        // Notify UI progress 
        emit scanProgress( m_scan_index, m_measures.length(), mag_ratio, phas_diff);
    }

    return 0;
}

int QAAPIQmlHWCalView::start_hwcal()
{
    AAPIMeasureList     measure_steps;
    unsigned int        freq, num_scans;
    int                 ret;

    /* Read number of scans*/
    num_scans = m_config->get_osl_n_scans();

    /* Prepare measurement steps */
    for( uint i = 0; i < CALIB_NUM_ENTRIES; i++ )
    {
        freq = AAPICalibrator::freq_by_index(i);

        AAPtr<AAPIMeasure> measure( AAPIMeasure::create(m_config, m_calibrator, this, freq, false, false, num_scans, false) );
        measure_steps.push_back( measure );
    }

    m_calibrator->scan_hwerror_begin();

    // reset scan index & cancel flag
    m_scan_index = 0;
    m_scan_cancelled = false;

    ret = start_measure( measure_steps );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    return 0;
}

void QAAPIQmlHWCalView::cancel_hwcal()
{
    m_scan_cancelled = true;
}
