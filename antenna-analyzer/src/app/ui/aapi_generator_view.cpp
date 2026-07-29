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


#include "aapi_generator_view.h"


///////////////////////////////////////////////////////////////////////////////
// class QAAPiGeneratorView
///////////////////////////////////////////////////////////////////////////////

QAAPiGeneratorView::QAAPiGeneratorView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                       AAPiGenerator *gen, AAPiCalibrator *cal,
                                       QAAPiMessages *msgs, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, msgs, parent)
    , m_calibrator(cal)
{
    m_measurementTimer = new QTimer(this);
    m_measurementTimer->setSingleShot(true);

    // FIX: Change slot reference from handleMeasurementTimer to handleMeasurement
    connect(m_measurementTimer, &QTimer::timeout, this, &QAAPiGeneratorView::handleMeasurement);
}

QAAPiGeneratorView::~QAAPiGeneratorView()
{
    m_measurementTimer->stop();
}

AAPiError QAAPiGeneratorView::onViewLoad()
{
    uint32_t measureFreq = m_config->get_measure_freq();

    // Validate measurement frequency
    if (! isFrequencyValid( measureFreq )) {
        measureFreq = 14'000'000;
    }

    m_frequency.store(static_cast<uint32_t>(measureFreq));

    // trigger a signal notifying QML of the new validated frequency
    emit frequencyChanged();

    return AAPI_SUCCESS;
}

AAPiError QAAPiGeneratorView::onViewActivate()
{
    qDebug() << "Generator Mode Active: Enabling RF Synth hardware at" << getFrequency() << "KHz";

    triggerMeasurement();

    return AAPI_SUCCESS;
}

void QAAPiGeneratorView::onViewDeactivate()
{
    qDebug() << "Generator Mode Deactivated";

    m_measurementTimer->stop();
}

AAPiError QAAPiGeneratorView::onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    if (measure == nullptr) {
        // re-arm the single shot loop anyway to keep the system running
        triggerMeasurement();
        return AAPI_SUCCESS;
    }

    // update properties
    m_measuredVmv = measure->mag_mv_v;
    m_measuredImv = measure->mag_mv_i;
    m_measuredPhaseDeg = measure->phase_diff_d;
    m_rawR = measure->Rx_raw.real();
    m_rawX = measure->Rx_raw.imag();
    m_oslR = measure->Rx.real();
    m_oslX = measure->Rx.imag();

    emit metricsMeasureFinished();

    qDebug() << "Measure complete";

    return AAPI_SUCCESS;
}

void QAAPiGeneratorView::onViewMeasureError(AAPiError error)
{
    emit metricsMeasureError(m_msgs->error(error));
}

void QAAPiGeneratorView::triggerMeasurement()
{
    if (!isActivated()) return;

    m_measurementTimer->start(200);
}

AAPiError QAAPiGeneratorView::startMetricsMeasure()
{
    int32_t numScans = qMin( m_config->get_measure_num_scans(), AAPI_MAX_MEASURE_SCANS );
    uint32_t measureFreq = m_frequency.load();
    AAPiMeasureTaskList steps;

    // Only single measure has to be performed
    AAPiPtr<AAPiMeasureTask> measure(
        AAPiMeasureTask::create( m_config, m_calibrator, this, measureFreq, true, true, numScans, false )
        );
    steps.push_back( std::move(measure) );

    // Start measurement sequence
    int ret = startMeasures( std::move(steps) );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiGeneratorView::handleMeasurement()
{
    AAPiError ret = startMetricsMeasure();
    if (AAPI_FAILED( ret )) {
        emit metricsMeasureError(m_msgs->error( ret ));

        // re-arm the single shot loop right away
        triggerMeasurement();
    }
}

void QAAPiGeneratorView::handleTuneFrequency(TuneDirection dir)
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

    int64_t newFreq = m_frequency.load();

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

    // Safety clamp using base class boundaries
    int64_t minLimit = getFrequencyMin();
    int64_t maxLimit = getFrequencyMax();
    if (newFreq < minLimit) newFreq = minLimit;
    if (newFreq > maxLimit) newFreq = maxLimit;

    // Prevent redundant processing if limits are hit and frequency didn't change
    if (m_frequency.load() == static_cast<uint32_t>(newFreq)) {
        return;
    }

    // Apply the update to the hardware
    m_frequency.store(static_cast<uint32_t>(newFreq));

    // Trigger a signal notifying QML of the new validated frequency
    emit frequencyChanged();
}

void QAAPiGeneratorView::handleDirectFreqInput(quint32 typedKHz)
{
    uint64_t newFreq = static_cast<uint64_t>(typedKHz) * 1000;

    // Clamp to limits
    int64_t minLimit = getFrequencyMin();
    int64_t maxLimit = getFrequencyMax();
    if (newFreq < minLimit) newFreq = minLimit;
    if (newFreq > maxLimit) newFreq = maxLimit;

    // Prevent redundant processing if limits are hit and frequency didn't change
    if (m_frequency.load() == static_cast<uint32_t>(newFreq)) {
        return;
    }

    m_frequency.store(static_cast<uint32_t>(newFreq));

    emit frequencyChanged();
}
