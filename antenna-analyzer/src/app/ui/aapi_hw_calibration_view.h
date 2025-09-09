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
                                    QObject *parent = Q_NULLPTR);
    ~QAAPiHWCalibrationView();

private:
    virtual int load_view();
    virtual void destroy_view();

// Measurement callback
    virtual int onViewMeasureFinished(AAPiMeasureTask *measure);

private:
    AAPiCalibrator  *m_calibrator;
    uint32_t        m_scanIndex;
    volatile bool   m_scanCancel;

signals:
    void scanProgress(int step, int total, double mag_ratio, double phase_diff);
    void scanNoSignal();

public slots:
    int start_hwcal();
    void cancel_hwcal();
};

#endif // UI_AAPI_HW_CALIBRATION_VIEW_H
