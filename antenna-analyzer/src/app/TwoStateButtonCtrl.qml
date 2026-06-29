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
import aapi


Button {
    id: control

    property bool on: false
    property string onText: "Stop"
    property string offText: "Start"

    text: offText
    state: "off"

    // Reads from your centralized themes, falling back to clean slate muted colors when disabled
    contentItem: Text {
        text: control.text
        font: control.font

        color: !control.enabled ? AapiTheme.style.buttonDisabledTextColor // Dimmed when disabled
               : (control.on   ? AapiTheme.style.buttonOnTextColor
                               : AapiTheme.style.buttonOffTextColor)

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 44

        // Mutes the background fill and borders to clearly signal that the hardware control is locked
        color: !control.enabled ? AapiTheme.style.buttonDisabledBackColor // Muted when disabled
               : (control.on    ? AapiTheme.style.buttonOnBackColor
                                : AapiTheme.style.buttonOffBackColor)

        border.color: !control.enabled ? AapiTheme.style.buttonDisabledBorderColor // Dimmed slate edge border
                      : (control.on    ? AapiTheme.style.buttonOnBorderColor
                                       : AapiTheme.style.buttonOffBorderColor)

        border.width: control.visualFocus ? 2 : 1
        radius: 4
    }

    states: [
        State {
            name: "off";
            PropertyChanges {
                target: control;
                on: false;
                text: offText;
            }
        },
        State {
            name: "on";
            PropertyChanges {
                target: control;
                on: true;
                text: onText;
            }
        }
    ]

    onClicked: {
        // Native QML automatically rejects mouse clicks if enabled is false,
        // but adding this structural check protects your callbacks against rogue script events
        if (!control.enabled) return;

        if (control.state === "on") {
            if (control.onStop)
                control.onStop();
            control.state = "off"
        }
        else if (handleStart()) {
            control.state = "on";
        }
    }

    function setOn() {
        control.state = "on"
    }
    function setOff() {
        control.state = "off"
    }

    function handleStart() {
        if (control.onStart)
            return control.onStart();
        else
            return true;
    }
}


/*Button {
    id: control

    property bool on: false
    property string onText: "Stop"
    property string offText: "Start"

    text: offText
    state: "off"

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.on ? AapiTheme.style.buttonOnTextColor
                          : AapiTheme.style.buttonOffTextColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    // Modern background component that automatically reads control.on changes
    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 44
        color: control.on ? AapiTheme.style.buttonOnBackColor
                          : AapiTheme.style.buttonOffBackColor
        border.color: control.on ? "#FF3344" : "#00CCAA"
        border.width: control.visualFocus ? 2 : 1
        radius: 4
    }

    states: [
        State {
            name: "off";
            PropertyChanges {
                target: control;
                on: false;
                text: offText;
            }
        },
        State {
            name: "on";
            PropertyChanges {
                target: control;
                on: true;
                text: onText;
            }
        }
    ]

    onClicked: {
        if (control.state === "on") {
            if (control.onStop)
                control.onStop();
            control.state = "off"
        }
        else if (handleStart()) {
            control.state = "on";
        }
    }

    function setOn() {
        control.state = "on"
    }
    function setOff() {
        control.state = "off"
    }

    function handleStart() {
        if (control.onStart)
            return control.onStart();
        else
            return true;
    }
}*/

