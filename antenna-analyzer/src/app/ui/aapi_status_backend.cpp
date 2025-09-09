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

#include "aapi_status_backend.h"


QAAPiStatusBackend::QAAPiStatusBackend(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_batteryStatus(BATTERY_UNKNOWN)
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(handleBatteryStatus()));
    m_timer->setSingleShot(false);
}

QAAPiStatusBackend::~QAAPiStatusBackend()
{
    destroy();
}

int QAAPiStatusBackend::init()
{
    if (m_timer->isActive())
        return 0;

    m_timer->start(5000);

    return 0;
}

void QAAPiStatusBackend::destroy()
{
    m_timer->stop();
}

void QAAPiStatusBackend::handleBatteryStatus()
{
    // TODO: check battery status

    if (m_batteryStatus == BATTERY_EMPTY)
        m_batteryStatus = BATTERY_FULL;
    else
        m_batteryStatus = BATTERY_EMPTY;

    emit batteryStatusChanged(m_batteryStatus);
}
