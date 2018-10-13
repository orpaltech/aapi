/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9

Item {
    property int stateCode: 0

    id: item
    states: [
        State {
            name: "success"
            PropertyChanges { target: item; stateCode: 2 }
            PropertyChanges { target: rect; color: "#11ff21" }
        },
        State {
            name: "busy"
            PropertyChanges { target: item; stateCode: 1 }
            PropertyChanges { target: rect; color: "#ff1121" }
        },
        State {
            name: "disabled"
            PropertyChanges { target: item; stateCode: 0 }
            PropertyChanges { target: rect; color: "#919191" }
        }
    ]

    Rectangle {
        id: rect
        anchors {
            fill: parent
            topMargin: item.height > item.width ? (item.height - item.width) / 2 : 0
            bottomMargin: anchors.topMargin
            leftMargin: item.width > item.height ? (item.width - item.height) / 2 : 0
            rightMargin: anchors.leftMargin
        }
        radius: width / 2
        border.color: "#333333"
    }

}
