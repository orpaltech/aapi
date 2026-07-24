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

#ifndef UI_AAPI_HW_CALIBRATION_VIEW_H
#define UI_AAPI_HW_CALIBRATION_VIEW_H

#include "aapi_view_backend.h"
#include "analyzer/aapi_calibrator.h"
#include "aapi_messages.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiHWCalibrationView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiHWCalibrationView class
///
class QAAPiHWCalibrationView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(HWCalibrationViewBackend)

public:
    explicit QAAPiHWCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                    AAPiGenerator *gen, AAPiCalibrator *cal,
                                    QAAPiMessages *msgs,
                                    QObject *parent = Q_NULLPTR);
    ~QAAPiHWCalibrationView();

private:
// QAAPiViewBackend
    AAPiError onViewLoad() override;
    void onViewDestroy() override;
    void onViewDeactivate() override;
    AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) override;
    void onViewMeasureError(AAPiError error) override;

private:
    AAPiPtr<AAPiCalibrator> m_calibrator;
    unsigned int    m_scanIndex;
    volatile bool   m_scanCancelled;

Q_SIGNALS:
    void scanProgress(int step, int total, quint32 freq, double mag_ratio, double phase_diff);
    void scanError(QString message);
    void scanNoSignal();

public Q_SLOTS:
    bool handleStartScan();
    void handleCancelScan();
};

#endif // UI_AAPI_HW_CALIBRATION_VIEW_H
