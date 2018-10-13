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
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

/**
 * DSP View
 */
SwipePage {
    backend: analyzer.dsp

    TabBar {
        id: tabBarDSP
        width: parent.width
        height: (parent.height/10)
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        /*font.pointSize: 8*/

        TabButton {
            text: "Spectrum"
            /*width: implicitWidth + leftPadding + rightPadding*/
        }

        TabButton {
            text: "Oscilloscope"
            /*width: implicitWidth + leftPadding + rightPadding*/
        }
    }

    StackLayout {
        id: tabsDSP
        anchors {
            top: tabBarDSP.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        currentIndex: tabBarDSP.currentIndex

        Loader {
            active: tabsDSP.currentIndex == 0
            sourceComponent: SpectrumTab {
                id: tabSpectrumDSP
            }
        }

        Loader {
            active: tabsDSP.currentIndex == 1
            sourceComponent: OscilloscopeTab {
                id: tabOscDSP
            }
        }
    }

    Connections {
        target: tabsDSP
        onCurrentIndexChanged: {
            backend.tab_changed(tabsDSP.currentIndex)
        }
    }
}
