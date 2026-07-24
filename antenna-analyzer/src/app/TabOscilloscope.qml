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
import QtQml
import QtGraphs
import QtQuick.Layouts
import QtQuick.Effects
import aapi
import ru.orpaltech.aapi

Item {
    id: tabOscilloscope
    width: 1280
    height: 620

    property SignalProcessViewBackend backend: aapi.view_signal_process

    property alias titleV: textLabelV.text
    property alias seriesV: lineSeriesV
    property alias titleI: textLabelI.text
    property alias seriesI: lineSeriesI

    Timer {
        id: timerOsc
        interval: 3000
        running: true
        repeat: true
        onTriggered: backend.handleWaveformUpdate()
    }

    ColumnLayout {
        id: col1
        spacing: 0
        anchors.fill: parent

        RowLayout {
            id: row1
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: containerV
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    id: visualWrapperV
                    anchors.fill: parent
                    color: "transparent"
                    z: 2

                    Item {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.top: parent.top
                        anchors.topMargin: 28

                        GraphsView {
                            id: chartV
                            anchors.fill: parent
                            antialiasing: true

                            theme: GraphsTheme {
                                theme: GraphsTheme.Theme.UserDefined
                                plotAreaBackgroundColor: AapiTheme.style.chart.plotAreaColor
                                backgroundColor: "transparent"

                                axisXLabelFont.pointSize: AapiTheme.style.chart.labelsXFontSize
                                axisYLabelFont.pointSize: AapiTheme.style.chart.labelsYFontSize

                                grid {
                                    mainColor: AapiTheme.style.chart.gridLineColor
                                    subColor: AapiTheme.style.chart.minorGridLineColor
                                }

                                axisX.mainColor: AapiTheme.style.chart.axisLineColor
                                axisY.mainColor: AapiTheme.style.chart.axisLineColor
                            }

                            axisX: ValueAxis { id: axisV_X }
                            axisY: ValueAxis { id: axisV_Y }

                            LineSeries {
                                id: lineSeriesV
                                width: 3
                                color: AapiTheme.style.chart.lineSeriesVColor
                            }
                        }
                    }
                }

                Text {
                    id: textLabelV
                    text: qsTr("Oscilloscope (V-channel)")
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    font.bold: true
                    font.pointSize: 11
                    color: chartV.theme.axisX.mainColor
                    z: 100
                }

                MultiEffect {
                    anchors.fill: visualWrapperV
                    source: visualWrapperV
                    shadowEnabled: AapiTheme.style.chart.dropShadowEnabled
                    shadowColor: Qt.rgba(0, 0, 0, 0.4)
                    shadowHorizontalOffset: 5
                    shadowVerticalOffset: 5
                    shadowBlur: 0.65
                    blurMultiplier: 1.0
                    autoPaddingEnabled: true
                    shadowScale: 1.01
                    z: 1
                }
            }
        }

        RowLayout {
            id: row2
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: containerI
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    id: visualWrapperI
                    anchors.fill: parent
                    color: "transparent"
                    z: 2

                    Item {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.top: parent.top
                        anchors.topMargin: 28

                        GraphsView {
                            id: chartI
                            anchors.fill: parent
                            antialiasing: true

                            theme: GraphsTheme {
                                theme: GraphsTheme.Theme.UserDefined
                                plotAreaBackgroundColor: AapiTheme.style.chart.plotAreaColor
                                backgroundColor: "transparent"

                                axisXLabelFont.pointSize: AapiTheme.style.chart.labelsXFontSize
                                axisYLabelFont.pointSize: AapiTheme.style.chart.labelsYFontSize

                                grid {
                                    mainColor: AapiTheme.style.chart.gridLineColor
                                    subColor: AapiTheme.style.chart.minorGridLineColor
                                }

                                axisX.mainColor: AapiTheme.style.chart.axisLineColor
                                axisY.mainColor: AapiTheme.style.chart.axisLineColor
                            }

                            axisX: ValueAxis { id: axisI_X }
                            axisY: ValueAxis { id: axisI_Y }

                            LineSeries {
                                id: lineSeriesI
                                width: 3
                                color: AapiTheme.style.chart.lineSeriesIColor
                            }
                        }
                    }
                }

                Text {
                    id: textLabelI
                    text: qsTr("Oscilloscope (I-channel)")
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    font.bold: true
                    font.pointSize: 11
                    color: chartI.theme.axisX.mainColor
                    z: 100
                }

                MultiEffect {
                    anchors.fill: visualWrapperI
                    source: visualWrapperI
                    shadowEnabled: AapiTheme.style.chart.dropShadowEnabled
                    shadowColor: Qt.rgba(0, 0, 0, 0.4)
                    shadowHorizontalOffset: 5
                    shadowVerticalOffset: 5
                    shadowBlur: 0.65
                    blurMultiplier: 1.0
                    autoPaddingEnabled: true
                    shadowScale: 1.01
                    z: 1
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("oscilloscope tab render complete")
        backend.handleWaveformSetup(lineSeriesV, lineSeriesI, axisV_X, axisV_Y, axisI_X, axisI_Y)
    }
}
