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

#ifndef UI_AAPI_MEASUREMENT_VIEW_H
#define UI_AAPI_MEASUREMENT_VIEW_H

#include "ui/aapi_view_backend.h"

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
    explicit QAAPiMeasurementView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                  AAPiGenerator *gen, QObject *parent = Q_NULLPTR);
    ~QAAPiMeasurementView();

private:
    virtual int load_view();
    virtual int activate_view();
    virtual void deactivate_view();
    virtual void destroy_view();

signals:

public slots:

};

#endif // UI_AAPI_MEASUREMENT_VIEW_H
