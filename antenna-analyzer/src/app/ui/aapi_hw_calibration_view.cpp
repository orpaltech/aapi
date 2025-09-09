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

#include "aapi_hw_calibration_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiHWCalibrationView
///////////////////////////////////////////////////////////////////////////////

QAAPiHWCalibrationView::QAAPiHWCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, AAPiCalibrator *cal,
                                               QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_calibrator(cal)
    , m_scanCancel(false)
{
    AAPI_ADDREF(m_calibrator);

    // Subscribe for DSP events 
    dsp->addCallback( this );
}

QAAPiHWCalibrationView::~QAAPiHWCalibrationView()
{
    AAPI_DISPOSE(m_calibrator);
}

int QAAPiHWCalibrationView::load_view()
{
    return 0;
}

void QAAPiHWCalibrationView::destroy_view()
{
}

int QAAPiHWCalibrationView::onViewMeasureFinished(AAPiMeasureTask *measure)
{
    double mag_ratio, phas_diff;
    int ret;

    // NOTE: We are in the main thread now 

    if (! measure)
    {
        // Finalize measurement
        m_calibrator->scan_hwe_ended();

        // Save calibration data to file
        ret = m_calibrator->flush_hwe_correction_file();
        // TODO: check error
    }
    else
    {
        if (m_scanCancel) /*User cancelled*/
        {
            return AAPI_E_CANCELLED;
        }

        if (measure->is_low_signal( ))
        {
            // Hardware problem 
            emit scanNoSignal();

            return AAPI_E_FAILURE;
        }

        mag_ratio = measure->mag_ratio;
        phas_diff = measure->Phas_diff;

        ret = m_calibrator->set_hwe_entry( m_scanIndex++, mag_ratio, phas_diff);
        // TODO: check error
        
        // Notify UI progress 
        emit scanProgress( m_scanIndex, m_measures.length(), mag_ratio, phas_diff);
    }

    return AAPI_SUCCESS;
}

int QAAPiHWCalibrationView::start_hwcal()
{
    AAPiMeasureTaskList     measure_steps;
    unsigned int            freq, num_scans;
    int                     ret;

    /* Read number of scans*/
    num_scans = m_config->get_osl_n_scans();

    /* Prepare measurement steps */
    for (uint i = 0; i < AAPI_CAL_NUM_ENTRIES; i++)
    {
        freq = AAPiCalibrator::freq_by_index( i );

        AAPiPtr<AAPiMeasureTask> measure( AAPiMeasureTask::create( m_config, m_calibrator, this, freq, false, false, num_scans, false ));
        measure_steps.push_back( measure );
    }

    m_calibrator->scan_hwe_begin();

    // reset scan index & cancel flag
    m_scanIndex = 0;
    m_scanCancel = false;

    ret = startMeasure( measure_steps );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiHWCalibrationView::cancel_hwcal()
{
    m_scanCancel = true;
}
