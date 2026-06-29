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
                                               QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_calibrator(cal)
    , m_scanCancelled(false)
{
    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiHWCalibrationView::~QAAPiHWCalibrationView()
{
}

AAPiError QAAPiHWCalibrationView::loadView()
{
    return AAPI_SUCCESS;
}

void QAAPiHWCalibrationView::destroyView()
{
}

void QAAPiHWCalibrationView::deactivateView()
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
        int ret = m_calibrator->flush_hw_err_correction_file();
        if (AAPI_FAILED( ret )) {
            // TODO: check error
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

        int ret = m_calibrator->set_hw_err_entry( m_scanIndex++, mag_ratio, phas_diff);
        if (AAPI_FAILED( ret )) {
            // TODO: check error
        }
    }

    return AAPI_SUCCESS;
}

AAPiError QAAPiHWCalibrationView::handleStartScan()
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
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiHWCalibrationView::handleCancelScan()
{
    m_scanCancelled = true;
}
