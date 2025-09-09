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

#ifndef UI_AAPI_STATUS_BACKEND_H
#define UI_AAPI_STATUS_BACKEND_H

#include <QObject>
#include <QTimer>
#include <QtQmlIntegration/qqmlintegration.h>

///////////////////////////////////////////////////////////////////////////////
// class QAAPiStatusBackend
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiStatusBackend class
///
class QAAPiStatusBackend : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(ApplicationStatusBackend)


    /* Properties */
    Q_PROPERTY(BatteryStatus battery_status READ getBatteryStatus NOTIFY batteryStatusChanged)

public:
    QAAPiStatusBackend(QObject *parent = Q_NULLPTR);
    ~QAAPiStatusBackend();

    enum BatteryStatus {
        BATTERY_UNKNOWN = 0,
        BATTERY_EMPTY,
        BATTERY_FULL,
        BATTERY_CHARGING,
        BATTERY_CHARGED,
    };

    /* Enumerations */
    Q_ENUM(BatteryStatus)

    BatteryStatus getBatteryStatus() const { return m_batteryStatus;}

    int init();
    void destroy();

signals:
    void batteryStatusChanged(BatteryStatus newStatus);

public slots:
    void handleBatteryStatus();

private:
    BatteryStatus   m_batteryStatus;
    QTimer          *m_timer;
};

#endif // UI_AAPI_STATUS_BACKEND_H
