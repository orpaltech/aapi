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
import QtQml 2.2
import QtCharts 2.2
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import com.orpaltech.aapi 1.0

PanVSWRTab {
    Row {
        id: rowButtonsVSWR
        spacing: 5
        anchors {
            margins: 5
            top: parent.top
            left: parent.left
            right: parent.right
        }
        width: parent.width

        Button {
            id: buttonFastVSWR
            text: "Fast VSWR"
            font.pointSize: 8
        }

        Button {
            id: buttonSlowVSWR
            text: "Slow VSWR"
            font.pointSize: 8
        }

        Button {
            id: buttonSnapshotVSWR
            text: "Snapshot"
            font.pointSize: 8
        }
    }

    ChartView {
        id: chartVSWR
        anchors {
            top: rowButtonsVSWR.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        theme: ChartView.ChartThemeQt
        backgroundColor: "#bdc3f5"
        backgroundRoundness: 0
        antialiasing: true
        legend {
            visible: false
            alignment: Qt.AlignTop
            font.pointSize: 7
        }
        margins {
            top: 2
            left: 0
            right: 0
            bottom: 2
        }

        ValueAxis {
            id: chartVSWRaxisX
            labelsFont.pointSize: 6
        }

        ValueAxis {
            id: chartVSWRaxisY
            labelsFont.pointSize: 6
        }

        SplineSeries {
            id: chartVSWRseries
            name: "VSWR"
            axisX: chartVSWRaxisX
            axisY: chartVSWRaxisY
            color: "#dc1e1e"
        }
    }


    Connections {
        target: chartVSWR
        onPlotAreaChanged: {
            console.log("new VSWR-chart plot area: " + chartVSWR.plotArea.width + ":" + chartVSWR.plotArea.height)
            backend.vswr_plot_area(chartVSWR.plotArea)
        }
    }

    Connections {
        target: buttonFastVSWR
        onClicked: {
            backend.vswr_fast_scan()
        }
    }

    Connections {
        target: buttonSlowVSWR
        onClicked: {
            backend.vswr_slow_scan()
        }
    }

    Connections {
        target: buttonSnapshotVSWR
        onClicked: {
            chartVSWR.grabToImage(function(result) {
                backend.vswr_snapshot(result)
                snapshotDialog.show()
            });
        }
    }

    onLoaded: {
        backend.vswr_setup(chartVSWRseries)
    }

    Dialog {
        id: snapshotDialog
        title: "New Snapshot"
        modal: true
        x: (Screen.width - snapshotDialog.width)/2
        y: (Screen.height - snapshotDialog.height)/2
        standardButtons: Dialog.Ok

        Label {
            id: labelMessage
            wrapMode: Text.WordWrap
        }

        function show() {
            labelMessage.text = "VSWR snapshot taken into: " + analyzer.lastSnapshot
            open()
        }
    }

    function enableControls(enable) {
        buttonFastVSWR.enabled = enable
        buttonSlowVSWR.enabled = enable
        buttonSnapshotVSWR.enabled = enable
    }
}
