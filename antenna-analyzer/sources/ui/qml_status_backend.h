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

#ifndef AAPI_QML_STATUS_BACKEND_H
#define AAPI_QML_STATUS_BACKEND_H

#include <QObject>
#include <QTimer>


class QAAPIQmlStatusBackend : public QObject
{
    Q_OBJECT

    /* Properties */
    Q_PROPERTY(BatteryStatus batteryStatus READ get_battery_status NOTIFY batteryStatusChanged)

public:
    QAAPIQmlStatusBackend();
    ~QAAPIQmlStatusBackend();

    enum BatteryStatus {
        BATTERY_EMPTY = 1,
        BATTERY_FULL = 2,
        BATTERY_CHARGING = 3,
        BATTERY_CHARGED = 4,
    };

    /* Enumerations */
    Q_ENUM(BatteryStatus)

    BatteryStatus get_battery_status() const { return m_batteryStatus;}

    int init();
    void destroy();
signals:
    void batteryStatusChanged();

public slots:
    void notifyBatteryStatusChanged();

private:
    BatteryStatus   m_batteryStatus;
    QTimer          *m_timer;
};

#endif // AAPI_QML_STATUS_BACKEND_H
