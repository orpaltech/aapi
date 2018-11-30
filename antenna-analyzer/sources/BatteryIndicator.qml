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

import QtQuick 2.11
import com.orpaltech.aapi 1.0

Item {
    id: batteryIndicator
    property StatusBackend backend: analyzer.status

    Connections {
        target: backend
        onBatteryStatusChanged: {
            iconEmpty.visible = (backend.batteryStatus == StatusBackend.BATTERY_EMPTY)
            iconFull.visible = (backend.batteryStatus == StatusBackend.BATTERY_FULL)
            iconCharging.visible = (backend.batteryStatus == StatusBackend.BATTERY_CHARGING)
            iconCharged.visible = (backend.batteryStatus == StatusBackend.BATTERY_CHARGED)
        }
    }

    Image {
        id: iconEmpty
        visible: backend.batteryStatus == StatusBackend.BATTERY_EMPTY
        anchors.fill: parent
        source: "qrc:/battery-empty.svg"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconFull
        visible: backend.batteryStatus == StatusBackend.BATTERY_FULL
        anchors.fill: parent
        source: "qrc:/battery-full.svg"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconCharging
        visible: backend.batteryStatus == StatusBackend.BATTERY_CHARGING
        anchors.fill: parent
        source: "qrc:/battery-half.svg"
        sourceSize {
            width: 32
            height: 25
        }
    }

    Image {
        id: iconCharged
        visible: backend.batteryStatus == StatusBackend.BATTERY_CHARGED
        anchors.fill: parent
        source: "qrc:/battery-three-quarters.svg"
        sourceSize {
            width: 32
            height: 25
        }
    }
}
