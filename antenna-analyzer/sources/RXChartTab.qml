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
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

PanVSWRTab {
    id: itemRXTab

    Row {
        id: rowButtonsRX
        spacing: 5
        anchors {
            margins: 5
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Button {
            id: buttonFastRX
            text: "Fast RX"
            font.pointSize: 8
        }

        Button {
            id: buttonSlowRX
            text: "Slow RX"
            font.pointSize: 8
        }

        Button {
            id: buttonSnapshotRX
            text: "Snapshot"
            font.pointSize: 8
        }
    }

    ChartView {
        id: chartRX
        anchors {
            top: rowButtonsRX.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        theme: ChartView.ChartThemeQt
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
            id: chartRXaxisX
            labelsFont.pointSize: 6
        }

        ValueAxis {
            id: chartRXaxisY
            labelsFont.pointSize: 6
        }

        SplineSeries {
            id: chartRXseriesR
            name: "R"
            axisX: chartRXaxisX
            axisY: chartRXaxisY
            color: "#1ec81e"
        }

        SplineSeries {
            id: chartRXseriesX
            name: "X"
            axisX: chartRXaxisX
            axisY: chartRXaxisY
            color: "#dc0e0e"
        }
    }

    Connections {
        target: chartRX
        onPlotAreaChanged: {
            console.log("new RX-chart plot area: " + chartRX.plotArea.width + ":" + chartRX.plotArea.height)
            backend.rx_plot_area(chartRX.plotArea)
        }
    }

    Connections {
        target: buttonFastRX
        onClicked: {
            backend.rx_fast_scan()
        }
    }

    Connections {
        target: buttonSlowRX
        onClicked: {
            backend.rx_slow_scan()
        }
    }

    Connections {
        target: buttonSnapshotRX
        onClicked: {
            chartRX.grabToImage(function(result) {
                backend.rx_snapshot(result)
                snapshotDialog.show()
            });
        }
    }

    onLoaded: {
        backend.rx_setup(chartRXseriesR, chartRXseriesX)
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
            labelMessage.text = "RX snapshot taken into: " + analyzer.lastSnapshot
            open()
        }
    }

    function enableControls(enable) {
        buttonFastRX.enabled = enable
        buttonSlowRX.enabled = enable
        buttonSnapshotRX.enabled = enable
    }
}
