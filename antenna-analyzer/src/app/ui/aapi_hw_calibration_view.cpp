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

#include "aapi_hw_calibration_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiHWCalibrationView
///////////////////////////////////////////////////////////////////////////////

QAAPiHWCalibrationView::QAAPiHWCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, AAPiCalibrator *cal,
                                               QAAPiMessages *msgs,
                                               QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, msgs, parent)
    , m_calibrator(cal)
    , m_scanCancelled(false)
{
    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiHWCalibrationView::~QAAPiHWCalibrationView()
{
}

AAPiError QAAPiHWCalibrationView::onViewLoad()
{
    return AAPI_SUCCESS;
}

void QAAPiHWCalibrationView::onViewDestroy()
{
}

void QAAPiHWCalibrationView::onViewDeactivate()
{
    cancelMeasures();
}

AAPiError QAAPiHWCalibrationView::onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    // NOTE: We are in the main thread now

    if (measure == nullptr) {
        // Finalize standard successful measurement
        m_calibrator->hw_err_scan_finalize();

        // Save calibration data to file
        auto ret = m_calibrator->flush_hw_err_correction_file();
        if (AAPI_FAILED( ret )) {
            emit scanError(m_msgs->error(ret));
        }

    } else {
        if (m_scanCancelled) { /*User cancelled*/
            return AAPI_E_CANCELLED;
        }

        uint32_t freq = measure->measure_freq;
        double mag_ratio = measure->mag_ratio;
        double phas_diff = measure->phase_diff;

        // Notify UI progress
        int step = m_scanIndex + 1; // Make step 1-based
        int total = getMaxMeasures();
        emit scanProgress( step, total, freq, mag_ratio, phas_diff);

        if (measure->is_low_signal()) {
            // Hardware problem
            emit scanNoSignal();

            return AAPI_E_FAILURE;
        }

        auto ret = m_calibrator->set_hw_err_entry( m_scanIndex++, mag_ratio, phas_diff);
        if (AAPI_FAILED( ret )) {
            emit scanError(m_msgs->error(ret));
        }
    }

    return AAPI_SUCCESS;
}

void QAAPiHWCalibrationView::onViewMeasureError(AAPiError error)
{
    // TODO: implement
}

bool QAAPiHWCalibrationView::handleStartScan()
{
    // Read number of scans
    uint32_t num_scans = qMin( m_config->get_calibr_num_scans(), AAPI_MAX_MEASURE_SCANS );

    AAPiMeasureTaskList steps;
    // Reserve memory upfront so the list never reallocates
    steps.reserve(AAPiCalibrator::NUM_ENTRIES);
    // Prepare measurement steps
    for (uint i = 0; i < AAPiCalibrator::NUM_ENTRIES; i++) {
        uint32_t freq = AAPiCalibrator::get_freq_by_index( i );

        AAPiPtr<AAPiMeasureTask> measure(
            AAPiMeasureTask::create( m_config, nullptr, this, freq, false, false, num_scans, false )
        );
        steps.push_back( std::move(measure) );
    }

    m_calibrator->hw_err_scan_begin();

    // reset scan index & cancel flag
    m_scanIndex = 0;
    m_scanCancelled = false;

    int ret = startMeasures( std::move(steps) );
    if (AAPI_FAILED( ret )) {
        emit scanError(m_msgs->error(ret));
        return false;
    }

    return true;
}

void QAAPiHWCalibrationView::handleCancelScan()
{
    m_scanCancelled = true;
}
