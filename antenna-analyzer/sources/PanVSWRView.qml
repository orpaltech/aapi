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
import QtQuick.Window 2.3
import QtCharts 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

SwipePage {
    backend: analyzer.panvswr

    TabBar {
        id: tabBarPanVSWR
        width: parent.width
        height: 22
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        font.pointSize: 12

        TabButton {
            text: "VSWR Chart"
            width: implicitWidth + leftPadding + rightPadding
        }

        TabButton {
            text: "RX Chart"
            width: implicitWidth + leftPadding + rightPadding
        }

        TabButton {
            text: "S11 Chart"
            width: implicitWidth + leftPadding + rightPadding
        }

        TabButton {
            text: "Smith Chart"
            width: implicitWidth + leftPadding + rightPadding
        }
    }

    StackLayout {
        id: tabsPanVSWR
        anchors {
            top: tabBarPanVSWR.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        currentIndex: tabBarPanVSWR.currentIndex

        VSWRChartTab {
            id: tabChartVSWR
        }

        RXChartTab {
            id: tabChartRX
        }

        PanVSWRTab {
            id: tabChartS11
        }

        SmithChartTab {
            id: tabChartSmith
        }

        Connections {
            target: tabsPanVSWR
            onCurrentIndexChanged: {
                backend.tab_changed(tabsPanVSWR.currentIndex)
            }
        }
    }


    /* ------------------------------------------------------------------- */
    /* The code below will handle state of the controls (enabled/disabled)
    /* ------------------------------------------------------------------- */
    function enableControls(enable) {
        for (var i=0; i< tabsPanVSWR.children.length; i++) {
            var tab = tabsPanVSWR.children[i];
            if (tab.enableControls)
                tab.enableControls(enable);
        }
    }

    Dialog {
        id: scanError
        title: "Scan Error"
        modal: true
        font.pointSize: 10
        x: (Screen.width - scanError.width)/2
        y: (Screen.height - scanError.height)/2
        standardButtons: Dialog.Ok

        Label {
            id: labelScanError
        }

        function show(message) {
            labelScanError.text = message
            open()
        }
    }

    Connections {
        target: backend
        onScanStarted: {
            enableControls(false)
        }

        onScanComplete: {
            enableControls(true)
        }

        onScanError: {
            console.log("Scan error: " + message)
            scanError.show("Error occurred during scan: " + message)
            enableControls(true)
        }

        onScanNoSignal: {
            scanError.show("Low signal. Please, check hardware.")
            enableControls(true)
        }
    }

    onLoaded: {
        for (var i=0; i< tabsPanVSWR.children.length; i++) {
            var tab = tabsPanVSWR.children[i];
            if (tab.loaded)
                tab.loaded();
        }
    }
}
