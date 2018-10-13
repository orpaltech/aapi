/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQml 2.2
import QtCharts 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

Item {
    property DSPViewBackend backend: analyzer.dsp

    property alias titleV: chartV.title
    property alias seriesV: lineSeriesV
    property alias titleI: chartI.title
    property alias seriesI: lineSeriesI

    Timer {
        id: timerOsc
        interval: 2000
        running: true
        repeat: true
        onTriggered: {
            console.log("oscilloscope timer triggered")
            backend.update_oscilloscope()
        }
    }

    ColumnLayout {
        id: layoutOsc
        anchors {
            fill: parent
            margins: 0
        }
        spacing: 0

        ChartView {
            id: chartV
            /*title: "Oscilloscope (V-channel)"
            titleFont.pixelSize: 10*/
            theme: ChartView.ChartThemeQt
            backgroundRoundness: 0
            antialiasing: true
            legend {
                visible: false
                font.pointSize: 7
            }
            margins {
                top: 2
                left: 0
                right: 0
                bottom: 2
            }
            Layout.fillWidth: true
            Layout.fillHeight: true

            ValueAxis {
                id: axisXV
                labelsFont.pointSize: 6
            }

            ValueAxis {
                id: axisYV
                labelsFont.pointSize: 6
            }

            LineSeries {
                id: lineSeriesV
                axisX: axisXV
                axisY: axisYV
                color: "#66cc00"
                name: "V-channel"
            }

        }

        ChartView {
            id: chartI
            /*title: "Oscilloscope (I-channel)"
            titleFont.pixelSize: 10*/
            theme: ChartView.ChartThemeQt
            backgroundRoundness: 0
            antialiasing: true
            legend {
                visible: false
                font.pointSize: 7
            }
            margins {
                top: 2
                left: 0
                right: 0
                bottom: 2
            }
            Layout.fillWidth: true
            Layout.fillHeight: true

            ValueAxis {
                id: axisXI
                labelsFont.pointSize: 6
            }

            ValueAxis {
                id: axisYI
                labelsFont.pointSize: 6
            }

            LineSeries {
                id: lineSeriesI
                axisX: axisXI
                axisY: axisYI
                color: "#0099ff"
                name: "I-channel"
            }
        }
    }

    Component.onCompleted: {
        backend.setup_oscilloscope(seriesV, seriesI)
        console.log("oscilloscope tab render complete")
    }
}
