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

#ifndef QML_ABOUT_VIEW_H
#define QML_ABOUT_VIEW_H

#include "qml_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// QAAPIQmlAboutView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlAboutView class
///
class QAAPIQmlAboutView
        : public QAAPIQmlView
{
    Q_OBJECT

public:
    explicit QAAPIQmlAboutView(AAPIConfig *config,
                               QObject *parent = Q_NULLPTR);
signals:

public slots:
    void quit();
    void reboot();
};

#endif // QML_ABOUT_VIEW_H
