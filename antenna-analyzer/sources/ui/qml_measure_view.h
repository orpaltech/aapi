/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef QML_MEASURE_VIEW_H
#define QML_MEASURE_VIEW_H

#include "ui/qml_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlMeasureView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlMeasureView class
///
class QAAPIQmlMeasureView : public QAAPIQmlView
{
    Q_OBJECT

public:
    explicit QAAPIQmlMeasureView(AAPIConfig *config,
                                AAPISignalProcessor *processor,
                                AAPIGenerator *gen,
                                QObject *parent = Q_NULLPTR);
    ~QAAPIQmlMeasureView();

private:
    virtual int load_view();
    virtual int activate_view();
    virtual void deactivate_view();
    virtual void destroy_view();

signals:

public slots:

};

#endif // QML_MEASURE_VIEW_H
