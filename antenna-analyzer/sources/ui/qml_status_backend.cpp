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

#include "qml_status_backend.h"


QAAPIQmlStatusBackend::QAAPIQmlStatusBackend()
{
    m_batteryStatus = BATTERY_EMPTY;
    m_timer = Q_NULLPTR;
}

QAAPIQmlStatusBackend::~QAAPIQmlStatusBackend()
{
    destroy();
}

int QAAPIQmlStatusBackend::init()
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(notifyBatteryStatusChanged()));
    m_timer->setSingleShot(false);
    m_timer->start(5000);

    return 0;
}

void QAAPIQmlStatusBackend::destroy()
{
    delete m_timer;
}

void QAAPIQmlStatusBackend::notifyBatteryStatusChanged()
{
    if (m_batteryStatus == BATTERY_EMPTY)
        m_batteryStatus = BATTERY_FULL;
    else
        m_batteryStatus = BATTERY_EMPTY;

    emit batteryStatusChanged();
}
