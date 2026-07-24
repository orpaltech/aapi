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

#ifndef AAPI_GENERATOR_VIEW_H
#define AAPI_GENERATOR_VIEW_H

#include "aapi_view_backend.h"
#include "analyzer/aapi_calibrator.h"
#include "aapi_messages.h"
#include <QTimer>
#include <atomic>

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiGeneratorView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiGeneratorView class
///
class QAAPiGeneratorView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(GeneratorViewBackend)

public:
    enum TuneDirection {
        TUNE_DOWN_LARGE  = -3, // <<< (e.g., -500 kHz)
        TUNE_DOWN_MEDIUM = -2, // <<  (e.g., -100 kHz)
        TUNE_DOWN_SMALL  = -1, // <   (e.g., -5 kHz)
        TUNE_UP_SMALL    = 1,  // >   (e.g., +5 kHz)
        TUNE_UP_MEDIUM   = 2,  // >>  (e.g., +100 kHz)
        TUNE_UP_LARGE    = 3   // >>> (e.g., +500 kHz)
    };
    Q_ENUM(TuneDirection)

    // Expose the live frequency to QML bindings
    Q_PROPERTY(quint32 frequency READ getFrequency NOTIFY frequencyChanged)

    // Expose your live measurement properties
    Q_PROPERTY(double measuredVmv READ measuredVmv NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double measuredImv READ measuredImv NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double measuredPhaseDeg READ measuredPhaseDeg NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double oslR READ oslR NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double oslX READ oslX NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double rawR READ rawR NOTIFY metricsMeasureFinished)
    Q_PROPERTY(double rawX READ rawX NOTIFY metricsMeasureFinished)

public:
    explicit QAAPiGeneratorView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                AAPiGenerator *gen, AAPiCalibrator *cal,
                                QAAPiMessages *msgs, QObject *parent = Q_NULLPTR);
    ~QAAPiGeneratorView();

    /* Property Accessors */
    quint32 getFrequency() const { return m_frequency.load() / 1000; }
    double measuredVmv() const { return m_measuredVmv; }
    double measuredImv() const { return m_measuredImv; }
    double measuredPhaseDeg() const { return m_measuredPhaseDeg; }
    double oslR() const { return m_oslR; }
    double oslX() const { return m_oslX; }
    double rawR() const { return m_rawR; }
    double rawX() const { return m_rawX; }

private:
    // QAAPiViewBackend
    AAPiError onViewLoad() override;
    AAPiError onViewActivate() override;
    void onViewDeactivate() override;
    AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) override;
    void onViewMeasureError(AAPiError error) override;

    void triggerMeasurement();
    AAPiError startMetricsMeasure();

Q_SIGNALS:
    void metricsMeasureFinished();
    void metricsMeasureError(QString message);
    void frequencyChanged();

public Q_SLOTS:
    void handleTuneFrequency(TuneDirection direction);
    void handleDirectFreqInput(quint32 freq);
    void handleMeasurement();

private:
    AAPiPtr<AAPiCalibrator> m_calibrator;
    std::atomic<uint32_t>   m_frequency;
    QTimer  *m_measurementTimer;

    // Member storage for live metrics
    double m_measuredVmv = 0.0;
    double m_measuredImv = 0.0;
    double m_measuredPhaseDeg = 0.0;
    double m_oslR = 0.0;
    double m_oslX = 0.0;
    double m_rawR = 0.0;
    double m_rawX = 0.0;
};


#endif // AAPI_GENERATOR_VIEW_H
