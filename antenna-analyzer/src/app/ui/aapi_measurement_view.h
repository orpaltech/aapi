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

#ifndef UI_AAPI_MEASUREMENT_VIEW_H
#define UI_AAPI_MEASUREMENT_VIEW_H

#include "ui/aapi_view_backend.h"
#include "aapi_messages.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiMeasurementView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiMeasurementView class
///
class QAAPiMeasurementView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(MeasurementViewBackend)

public:
    enum TuneDirection {
        TUNE_DOWN_LARGE  = -3, // <<< (e.g., -100 kHz)
        TUNE_DOWN_MEDIUM = -2, // <<  (e.g., -10 kHz)
        TUNE_DOWN_SMALL  = -1, // <   (e.g., -1 kHz)
        TUNE_UP_SMALL    = 1,  // >   (e.g., +1 kHz)
        TUNE_UP_MEDIUM   = 2,  // >>  (e.g., +10 kHz)
        TUNE_UP_LARGE    = 3   // >>> (e.g., +100 kHz)
    };
    Q_ENUM(TuneDirection)

    // Expose the live frequency to QML bindings
    Q_PROPERTY(quint32 frequency READ getCurrentFrequency NOTIFY currentFrequencyChanged)

public:
    explicit QAAPiMeasurementView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                  AAPiGenerator *gen, AAPiCalibrator *cal,
                                  QAAPiMessages *msgs,
                                  QObject *parent = Q_NULLPTR);
    ~QAAPiMeasurementView();

    /* Property Accessors */
    quint32 getCurrentFrequency() const { return m_frequency / 1000; }

private:
// QAAPiViewBackend
    AAPiError loadView() override;
    void destroyView() override;
    AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) override;
    void onViewMeasureError(AAPiError error) override;

    AAPiError startMetricsMeasure();

Q_SIGNALS:
    void metricsMeasureFinished(double vswr, double r_real, double x_imag, double return_loss, double phase_deg, double component_val, bool is_inductive);
    void currentFrequencyChanged();
    void measureError(QString message);

public Q_SLOTS:
    void handleTuneFrequency(TuneDirection direction);
    void handleDirectFreqInput(quint32 freq);

private:
    AAPiPtr<AAPiCalibrator> m_calibrator;
    uint32_t                m_frequency;

    QAAPiMessages           *m_msgs;
};

#endif // UI_AAPI_MEASUREMENT_VIEW_H
