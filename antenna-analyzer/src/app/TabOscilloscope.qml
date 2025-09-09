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
import QtQml
import QtCharts
import QtQuick.Layouts
import aapi
import ru.orpaltech.aapi


Item {
    id: tabOscilloscope
    width: 1280
    height: 620

    property SignalProcessViewBackend backend: aapi.view_signal_process
    property alias titleV: chartV.title
    property alias seriesV: lineSeriesV
    property alias titleI: chartI.title
    property alias seriesI: lineSeriesI

    Timer {
        id: timerOsc
        interval: 2000
        running: true
        repeat: true
        onTriggered: backend.update_oscilloscope()
    }

    ColumnLayout {
        id: col1
        spacing: 0


        RowLayout {
            id: row1
            spacing: 0

            ChartView {
                id: chartV
                width: 1280
                height: 310
                title: qsTr("Oscilloscope (V-channel)")
                antialiasing: true
                legend {
                    visible: false
                    font.pointSize: 7
                }
                dropShadowEnabled: AapiTheme.style.chart.dropShadowEnabled
                backgroundRoundness: 0
                plotAreaColor: AapiTheme.style.chart.plotAreaColor

                axes: [
                    ValueAxis {
                        id: axisV_X
                        labelsFont.pointSize: AapiTheme.style.chart.labelsXFontSize
                        color: AapiTheme.style.chart.axisLineColor
                        gridVisible: true
                        gridLineColor: AapiTheme.style.chart.gridLineColor
                        minorGridVisible: true
                        minorGridLineColor: AapiTheme.style.chart.minorGridLineColor
                    },
                    ValueAxis {
                        id: axisV_Y
                        labelsFont.pointSize: AapiTheme.style.chart.labelsYFontSize
                        color: AapiTheme.style.chart.axisLineColor
                        gridVisible: true
                        gridLineColor: AapiTheme.style.chart.gridLineColor
                        minorGridVisible: true
                        minorGridLineColor: AapiTheme.style.chart.minorGridLineColor
                    }
                ]

                LineSeries {
                    id: lineSeriesV
                    axisX: axisV_X
                    axisY: axisV_Y
                    color: AapiTheme.style.chart.lineSeriesVColor
                    name: qsTr("V-channel")
                    pointLabelsVisible: false
                }
            }
        }

        RowLayout {
            id: row2
            spacing: 0

            ChartView {
                id: chartI
                width: 1280
                height: 310
                title: qsTr("Oscilloscope (I-channel)")
                antialiasing: true
                legend {
                    visible: false
                    font.pointSize: 7
                }
                dropShadowEnabled: AapiTheme.style.chart.dropShadowEnabled
                backgroundRoundness: 0
                plotAreaColor: AapiTheme.style.chart.plotAreaColor

                axes: [
                    ValueAxis {
                        id: axisI_X
                        labelsFont.pointSize: AapiTheme.style.chart.labelsXFontSize
                        color: AapiTheme.style.chart.axisLineColor
                        gridVisible: true
                        gridLineColor: AapiTheme.style.chart.gridLineColor
                        minorGridVisible: true
                        minorGridLineColor: AapiTheme.style.chart.minorGridLineColor
                    },
                    ValueAxis {
                        id: axisI_Y
                        labelsFont.pointSize: AapiTheme.style.chart.labelsYFontSize
                        color: AapiTheme.style.chart.axisLineColor
                        gridVisible: true
                        gridLineColor: AapiTheme.style.chart.gridLineColor
                        minorGridVisible: true
                        minorGridLineColor: AapiTheme.style.chart.minorGridLineColor
                    }
                ]

                LineSeries {
                    id: lineSeriesI
                    axisX: axisI_X
                    axisY: axisI_Y
                    color: AapiTheme.style.chart.lineSeriesIColor
                    name: qsTr("I-channel")
                    pointLabelsVisible: false
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("oscilloscope tab render complete")
        backend.setup_oscilloscope(seriesV, seriesI)
    }
}
