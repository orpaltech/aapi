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

import QtQuick
import aapi
import ru.orpaltech.aapi

Item {
    id: batteryInd
    width: 32
    height: 25
    property ApplicationStatusBackend backend: aapi.status


    Image {
        id: iconEmpty
        visible: backend.battery_status == ApplicationStatusBackend.BATTERY_EMPTY
        anchors.fill: parent
        source: "/battery/empty"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconFull
        visible: backend.battery_status == ApplicationStatusBackend.BATTERY_FULL
        anchors.fill: parent
        source: "/battery/full"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconCharging
        visible: backend.battery_status == ApplicationStatusBackend.BATTERY_CHARGING
        anchors.fill: parent
        source: "/battery/half-power"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconCharged
        visible: backend.battery_status == ApplicationStatusBackend.BATTERY_CHARGED
        anchors.fill: parent
        source: "/battery/three-quarters"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Connections {
        target: backend
        function onBatteryStatusChanged(newStatus) {
            iconEmpty.visible = (backend.battery_status == ApplicationStatusBackend.BATTERY_EMPTY)
            iconFull.visible = (backend.battery_status == ApplicationStatusBackend.BATTERY_FULL)
            iconCharging.visible = (backend.battery_status == ApplicationStatusBackend.BATTERY_CHARGING)
            iconCharged.visible = (backend.battery_status == ApplicationStatusBackend.BATTERY_CHARGED)
        }
    }
}
