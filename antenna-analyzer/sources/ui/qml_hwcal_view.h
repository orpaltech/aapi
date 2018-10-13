/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef QML_HWCAL_VIEW_H
#define QML_HWCAL_VIEW_H

#include "qml_view_backend.h"
#include "analyzer/analyzer_calibrator.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlHWCalView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlHWCalView class
///
class QAAPIQmlHWCalView
        : public QAAPIQmlView
{
    Q_OBJECT

public:
    explicit QAAPIQmlHWCalView(AAPIConfig *config, AAPISignalProcessor *processor,
                               AAPIGenerator *gen, AAPICalibrator *cal,
                               QObject *parent = Q_NULLPTR);
    ~QAAPIQmlHWCalView();

private:
    /* Measurement callback */
    virtual int on_measure_finished(aapi_measure *measure);

    virtual int load_view();
    virtual void destroy_view();

private:
    aapi_ptr<AAPICalibrator> m_calibrator;
    uint32_t        m_scanIndex;
    volatile bool   m_scanCancelled;

signals:
    void scanProgress(int step, int total, float mag_ratio, float phase_diff);
    void scanNoSignal();

public slots:
    int start_hwcal();
    void cancel_hwcal();
};

#endif // QML_HWCAL_VIEW_H
