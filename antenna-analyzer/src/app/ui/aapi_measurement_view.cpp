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

#include "aapi_measurement_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiMeasurementView
///////////////////////////////////////////////////////////////////////////////

QAAPiMeasurementView::QAAPiMeasurementView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                           AAPiGenerator *gen, AAPiCalibrator *cal,
                                           QAAPiMessages *msgs, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, msgs, parent)
    , m_calibrator(cal)
{
    // Subscribe for DSP events
    dsp->addCallback(this);
}

QAAPiMeasurementView::~QAAPiMeasurementView()
{
}

AAPiError QAAPiMeasurementView::onViewLoad()
{
    uint32_t measure_freq = m_config->get_measure_freq();

    // Validate measurement frequency
    if (! isFrequencyValid( measure_freq )) {
        measure_freq = 14'000'000;
    }

    m_frequency = measure_freq;

    // trigger a signal notifying QML of the new validated frequency
    emit frequencyChanged();

    int oslFile = m_config->get_osl_file_selected();
    if (oslFile < 0) {
        // No OSL-calibration file selected
        return AAPI_E_FAILURE;
    }

    return AAPI_SUCCESS;
}

void QAAPiMeasurementView::onViewDestroy()
{
}

AAPiError QAAPiMeasurementView::onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    if (measure == nullptr) {
        // Measurement complete, do nothing
        return AAPI_SUCCESS;
    }

    qDebug() << "EMITTING DATA TO QML:"
             << "RL:" << measure->return_loss_db
             << "Phase:" << measure->phase_diff_d
             << "Val:" << measure->component_val;

    emit metricsMeasureFinished(measure->vswr,
                                measure->Rx.real(), measure->Rx.imag(),
                                measure->return_loss_db,
                                measure->phase_diff_d,
                                measure->component_val, measure->is_inductive);
    return AAPI_SUCCESS;
}

void QAAPiMeasurementView::onViewMeasureError(AAPiError error)
{
    emit metricsMeasureError(m_msgs->error(error));
}

AAPiError QAAPiMeasurementView::startMetricsMeasure()
{
    int32_t num_scans = qMin( m_config->get_measure_num_scans(), AAPI_MAX_MEASURE_SCANS );
    AAPiMeasureTaskList steps;
    // Only single measure has to be performed
    AAPiPtr<AAPiMeasureTask> measure(
        AAPiMeasureTask::create( m_config, m_calibrator, this, m_frequency, true, true, num_scans, false )
        );
    steps.push_back( std::move(measure) );

    // Start measurement sequence
    int ret = startMeasures( std::move(steps) );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    return AAPI_SUCCESS;
}

/*void QAAPiMeasurementView::handleTuneFrequency(TuneDirection dir)
{
    uint32_t stepHz = 0;

    switch (dir) {
    case TUNE_DOWN_SMALL:
    case TUNE_UP_SMALL:
        stepHz = 5000;      // 5 kHz
        break;
    case TUNE_DOWN_MEDIUM:
    case TUNE_UP_MEDIUM:
        stepHz = 100'000;   // 100 kHz
        break;
    case TUNE_DOWN_LARGE:
    case TUNE_UP_LARGE:
        stepHz = 500'000;   // 500 kHz
        break;
    }

    // Determine addition or subtraction
    int64_t newFreq = m_frequency;
    if (dir < 0) {
        newFreq -= stepHz;
    } else {
        newFreq += stepHz;
    }

    // Safety clamp using base class boundaries
    int64_t minLimit = getFrequencyMin();
    int64_t maxLimit = getFrequencyMax();
    if (newFreq < minLimit) newFreq = minLimit;
    if (newFreq > maxLimit) newFreq = maxLimit;

    // Apply the update to the hardware
    m_frequency = static_cast<uint32_t>(newFreq);

    // trigger a signal notifying QML of the new validated frequency
    emit currentFrequencyChanged();

    AAPiError ret = startMetricsMeasure();
    if (AAPI_FAILED( ret )) {
        emit metricsMeasureError(m_msgs->error( ret ));
    }
}*/

void QAAPiMeasurementView::handleTuneFrequency(TuneDirection dir)
{
    uint32_t stepHz = 0;

    switch (dir) {
    case TUNE_DOWN_SMALL:
    case TUNE_UP_SMALL:
        stepHz = 5000;      // 5 kHz
        break;
    case TUNE_DOWN_MEDIUM:
    case TUNE_UP_MEDIUM:
        stepHz = 100'000;   // 100 kHz
        break;
    case TUNE_DOWN_LARGE:
    case TUNE_UP_LARGE:
        stepHz = 500'000;   // 500 kHz
        break;
    }

    if (stepHz == 0) return;

    int64_t newFreq = m_frequency;

    if (dir == TUNE_DOWN_SMALL || dir == TUNE_DOWN_MEDIUM || dir == TUNE_DOWN_LARGE) {

        // Apply Yury Kuchura's original step grid alignment constraint
        if (newFreq > stepHz && (newFreq % stepHz) != 0) {
            newFreq -= (newFreq % stepHz); // Snap down to the nearest even block boundary
        } else {
            newFreq -= stepHz;
        }

    } else { // Handle up directions safely

        if (newFreq % stepHz != 0) {
            newFreq -= (newFreq % stepHz); // Clear remainder grid lines first
        }
        newFreq += stepHz;
    }

    // 3. Safety clamp using base class boundaries
    int64_t minLimit = getFrequencyMin();
    int64_t maxLimit = getFrequencyMax();
    if (newFreq < minLimit) newFreq = minLimit;
    if (newFreq > maxLimit) newFreq = maxLimit;

    // Prevent redundant processing if limits are hit and frequency didn't change
    if (m_frequency == static_cast<uint32_t>(newFreq)) {
        return;
    }

    // Apply the update to the hardware
    m_frequency = static_cast<uint32_t>(newFreq);

    // Trigger a signal notifying QML of the new validated frequency
    emit frequencyChanged();

    AAPiError ret = startMetricsMeasure();
    if (AAPI_FAILED( ret )) {
        emit metricsMeasureError(m_msgs->error( ret ));
    }
}

void QAAPiMeasurementView::handleDirectFreqInput(quint32 typedKHz)
{
    // Convert direct user typing from KHz back up to raw Hz
    uint64_t newFreq = static_cast<uint64_t>(typedKHz) * 1000;

    // Clamp to limits
    int64_t minLimit = getFrequencyMin();
    int64_t maxLimit = getFrequencyMax();
    if (newFreq < minLimit) newFreq = minLimit;
    if (newFreq > maxLimit) newFreq = maxLimit;

    // Prevent redundant processing if limits are hit and frequency didn't change
    if (m_frequency == static_cast<uint32_t>(newFreq)) {
        return;
    }

    m_frequency = static_cast<uint32_t>(newFreq);

    // trigger a signal notifying QML of the new validated frequency
    emit frequencyChanged();

    AAPiError ret = startMetricsMeasure();
    if (AAPI_FAILED( ret )) {
        emit metricsMeasureError(m_msgs->error( ret ));
    }
}
