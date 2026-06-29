/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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
    id: item
    property int stateCode: 0

    implicitWidth: 36
    implicitHeight: 36

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
                visible: true
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
                visible: false // Hide the solid dot so the spinner pops
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
                visible: true
            }
        }
    ]

    // Only displays and spins when busy
    Canvas {
        id: bootstrapSpinner
        anchors.fill: parent
        visible: item.state === "busy"

        // Anti-aliasing for smooth, crisp rendering edges
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var centerX = width / 2;
            var centerY = height / 2;

            // Calculate a safe radius that fits within the item square size boundaries
            var thickness = 3.5;
            var radius = (Math.min(width, height) / 2) - (thickness / 2) - 1;

            // Draw the open arc line layout segment (Leave a clear 90-degree gap)
            ctx.beginPath();
            ctx.arc(centerX, centerY, radius, 0, 1.5 * Math.PI, false); // 0 to 270 degrees

            ctx.strokeStyle = AapiTheme.style.statusBusyColor;
            ctx.lineWidth = thickness;
            ctx.lineCap = "round"; // Rounded ends just like standard bootstrap spinners
            ctx.stroke();
        }

        // Re-draw the vector tracking canvas container if resolution frames size toggles
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        // Continuous structural velocity rotational animation engine
        RotationAnimator {
            target: bootstrapSpinner
            from: 0
            to: 360
            duration: 750 // 750ms rotation speed matches real bootstrap metrics nicely
            running: item.state === "busy"
            loops: Animation.Infinite
        }
    }

    // Active during Success and Disabled phases
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
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
