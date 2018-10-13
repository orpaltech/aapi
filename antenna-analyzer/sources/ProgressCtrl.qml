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
    property int value: 0

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


    width: 250; height: 23
    clip: true

    Rectangle {
        id: highlight

        /*!
            An internal documentation comment. The widthDest property is not
            a public API and therefore will not be exposed.
        */
        property int widthDest: ((progressctrl.width * (value - minimum)) / (maximum - minimum) - 6)

        width: highlight.widthDest
        Behavior on width { SmoothedAnimation { velocity: 1200 } }

        anchors { left: parent.left; top: parent.top; bottom: parent.bottom; margins: 3 }
        radius: 1
        gradient: Gradient {
            GradientStop { id: gradient1; position: 0.0 }
            GradientStop { id: gradient2; position: 1.0 }
        }

    }

    Text {
        anchors { right: highlight.right; rightMargin: 6; verticalCenter: parent.verticalCenter }
        color: "white"
        font.bold: true
        text: Math.floor((value - minimum) / (maximum - minimum) * 100) + '%'
    }
}
