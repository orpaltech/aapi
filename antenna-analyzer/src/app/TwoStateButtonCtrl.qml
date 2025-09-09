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
import QtQuick.Controls

Button {
    property bool on: false
    property string onText: "Stop"
    property string offText: "Start"

    id: button
    text: offText

    states: [
        State {
            name: "off";
            PropertyChanges {
                target: button;
                on: false;
                text: offText;
            }
        },
        State {
            name: "on";
            PropertyChanges {
                target: button;
                on: true;
                text: onText;
            }
        }
    ]

    onClicked: {
        if (button.state === "on") {
            if (button.onStop)
                button.onStop();
            button.state = "off"
        }
        else if (handleStart()) {
            button.state = "on";
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
