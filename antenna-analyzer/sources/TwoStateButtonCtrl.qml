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
import QtQuick.Controls 2.2

Button {
    property bool on: false
    property string onText: "Stop"
    property string offText: "Start"

    id: button
    text: offText

    states: [
        State {
            name:"off";
            PropertyChanges { target: button; on: false; text: offText; }
        },
        State {
            name:"on";
            PropertyChanges { target: button; on: true; text: onText; }
        }
    ]

    onClicked: {
        if (button.state == "on") {
            if (button.onStop)
                button.onStop();
            button.state = "off"
        }
        else {
            if (handleStart())
                button.state = "on"
        }
    }

    function setOn() {
        button.state = "on"
    }
    function setOff() {
        button.state = "off"
    }

    function handleStart() {
        if (button.onStart)
            return button.onStart();
        else
            return true;
    }
}
