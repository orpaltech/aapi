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

Item {
    property int stateCode: 0

    id: item
    states: [
        State {
            name: "success"
            PropertyChanges {
                target: item;
                stateCode: 2
            }
            PropertyChanges {
                target: circle;
                color: AapiTheme.style.statusSuccessColor
            }
        },
        State {
            name: "busy"
            PropertyChanges {
                target: item;
                stateCode: 1
            }
            PropertyChanges {
                target: circle;
                color: AapiTheme.style.statusBusyColor
            }
        },
        State {
            name: "disabled"
            PropertyChanges {
                target: item;
                stateCode: 0
            }
            PropertyChanges {
                target: circle;
                color: AapiTheme.style.statusDisableColor
            }
        }
    ]

    Rectangle {
        id: circle
        anchors {
            fill: parent
            topMargin: item.height > item.width ? (item.height - item.width) / 2 : 0
            bottomMargin: anchors.topMargin
            leftMargin: item.width > item.height ? (item.width - item.height) / 2 : 0
            rightMargin: anchors.leftMargin
        }
        radius: width / 2
        border.color: AapiTheme.style.controlBorderColor
    }

}
