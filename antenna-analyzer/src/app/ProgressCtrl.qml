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


Item {
    id: progressctrl
    /*!
        The minimum value of the ProgressBar range.
        The \l value must not be less than this value.
    */
    property int minimum: 0

    /*!
        The maximum value of the ProgressBar range.
        The \l value must not be more than this value.
    */
    property int maximum: 100

    /*!
        The value of the progress.
    */
    property int value: 35

    /*!
       \qmlproperty color ProgressBar::color
       The color of the ProgressBar's gradient. Must bind to a color type.

       \omit
           The "\qmlproperty <type> <property name>" is needed because
           property alias need to have their types manually entered.

           QDoc will not publish the documentation within omit and endomit.
       \endomit

       \sa secondColor
    */
    property alias color: gradient1.color

    /*!
       \qmlproperty color ProgressBar::secondColor
       The second color of the ProgressBar's gradient.
       Must bind to a color type.

       \omit
           The "\qmlproperty <type> <property name>" is needed because
           property alias need to have their types manually entered.

           QDoc will not publish the documentation within omit and endomit.
       \endomit

        \sa color
    */
    property alias secondColor: gradient2.color

    /*property alias borderWidth: background.border.width
    property alias borderColor: background.border.color
    property alias textSize: percentage.font.pointSize*/


    width: 250
    height: 28
    clip: true
    color: AapiTheme.style.barGradientColor1
    secondColor: AapiTheme.style.barGradientColor2

    Rectangle {
        id: background
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color:AapiTheme.style.controlBorderColor
    }

    Rectangle {
        id: highlight

        /*!
            An internal documentation comment. The widthDest property is not
            a public API and therefore will not be exposed.
        */
        property int widthDest: ((progressctrl.width * (value - minimum)) / (maximum - minimum) - 6)

        width: highlight.widthDest
        Behavior on width {
            SmoothedAnimation { velocity: 1200 }
        }

        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
            margins: 3
        }
        radius: 1
        gradient: Gradient {
            GradientStop {
                id: gradient1;
                position: 0.0
            }
            GradientStop {
                id: gradient2;
                position: 1.0
            }
        }

    }

    Text {
        id: percentage
        anchors {
            right: highlight.right;
            rightMargin: 6;
            verticalCenter: parent.verticalCenter
        }
        color: "#0e2120"
        font.bold: true
        font.pointSize: 12
        text: Math.floor((value - minimum) / (maximum - minimum) * 100) + '%'
    }
}
