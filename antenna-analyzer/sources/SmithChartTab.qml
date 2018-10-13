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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import com.orpaltech.aapi 1.0

PanVSWRTab {
    id: itemSmith

    Column {
        id: colButtons
        spacing: 6
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 5
        }

        Button {
            id: buttonFast
            text: "Fast Smith"
            font.pointSize: 8
            width: buttonSlow.width
        }

        Button {
            id: buttonSlow
            text: "Slow Smith"
            font.pointSize: 8
        }

        Button {
            id: buttonSnapshot
            text: "Snapshot"
            font.pointSize: 8
            width: buttonSlow.width
        }
    }

    SmithChart {
        id: chartSmith
        anchors {
            left: colButtons.right
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 0
        }
    }

    /*
    PolarChartView {
        id: chartSmith
        anchors {
            left: colButtons.right
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 0
        }
        theme: PolarChartView.ChartThemeQt
        antialiasing: true
        legend {
            visible: false
            font.pointSize: 7
        }
        margins {
            top: 5
            left: 0
            right: 0
            bottom: 5
        }
        plotAreaColor: "#567869"

        ValueAxis {
            id: axisAngular
            labelsFont.pointSize: 6
            gridVisible: false
            minorGridVisible: false
            labelsVisible: true
            min: 0
            max: 360
        }

        ValueAxis {
            id: axisRadial
            labelsFont.pointSize: 6
            gridVisible: false
            minorGridVisible: false
            labelsVisible: false
            lineVisible: false
        }

        SplineSeries {
            id: seriesSmith
            axisAngular: axisAngular
            axisRadial: axisRadial
            pointsVisible: true
        }
    }*/

    Connections {
        target: chartSmith
        onPlotAreaChanged: {
            console.log("new Smith-chart plot area: " + chartSmith.plotArea.width + ":" + chartSmith.plotArea.height)
            backend.smith_plot_area(chartSmith.plotArea)
        }
    }

    Connections {
        target: buttonFast
        onClicked: {
            backend.smith_fast_scan()
        }
    }

    Connections {
        target: buttonSlow
        onClicked: {
            backend.smith_slow_scan()
        }
    }

    Connections {
        target: buttonSnapshot
        onClicked: {
            /*chartRX.grabToImage(function(result) {
                backend.rx_snapshot(result)
                snapshotDialog.show()
            });*/
        }
    }

    onLoaded: {
        backend.smith_setup(chartSmith)
    }

    function enableControls(enable) {
        buttonFast.enabled = enable
        buttonSlow.enabled = enable
        buttonSnapshot.enabled = enable
    }
}
