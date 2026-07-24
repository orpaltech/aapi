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
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import QtQuick.Effects
import aapi
import ru.orpaltech.aapi

Item {
    id: tabRXChartRoot
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    readonly property string seriesRColor: "#1ec81e"
    readonly property string seriesXColor: "#dc0e0e"
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    ColumnLayout {
        id: column1
        anchors.fill: parent

        Item {
            id: row1
            Layout.fillWidth: true
            Layout.preferredHeight: 36

            RowLayout {
                id: rowButtons
                spacing: 8
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.topMargin: 2

                Button {
                    id: btnFastRX
                    text: qsTr("Fast RX")
                    rightPadding: 10
                    leftPadding: 10
                    font.bold: true
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                Button {
                    id: btnSlowRX
                    text: qsTr("Slow RX")
                    rightPadding: 10
                    leftPadding: 10
                    font.bold: true
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                Button {
                    id: btnSnapshotRX
                    text: qsTr("Snapshot")
                    rightPadding: 10
                    leftPadding: 10
                    highlighted: false
                    font.bold: true
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                // Absorbs the remaining row space to prevent button stretching
                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            id: row2
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                id: chartContainer
                anchors.fill: parent

                Rectangle {
                    id: chartVisualBox
                    anchors.fill: parent
                    color: "transparent"
                    z: 2

                    GraphsView {
                        id: chartRX
                        anchors.fill: parent
                        z: 3
                        antialiasing: true
                        layer.enabled: true

                        visible: plotArea.width > 0

                        theme: GraphsTheme {
                            theme: GraphsTheme.Theme.UserDefined
                            plotAreaBackgroundColor: AapiTheme.style.chart.plotAreaColor
                            backgroundColor: "transparent"
                            axisXLabelFont.pointSize: 6
                            axisYLabelFont.pointSize: 6

                            axisX.mainColor: AapiTheme.style.chart.axisLineColor
                            axisX.subColor: AapiTheme.style.chart.gridLineColor

                            axisY.mainColor: AapiTheme.style.chart.axisLineColor
                            axisY.subColor: AapiTheme.style.chart.gridLineColor

                            grid {
                                mainColor: AapiTheme.style.chart.gridLineColor
                                subColor: AapiTheme.style.chart.minorGridLineColor
                            }
                        }

                        axisX: ValueAxis {
                            id: chartRXaxisX
                        }

                        axisY: ValueAxis {
                            id: chartRXaxisY
                        }

                        LineSeries {
                            id: chartRXseriesR
                            width: 2.2
                            color: seriesRColor
                        }

                        LineSeries {
                            id: chartRXseriesX
                            width: 2.2
                            color: seriesXColor
                        }
                    }
                }

                Item {
                    id: backgroundBandsLayer
                    x: chartRX.plotArea.x
                    y: chartRX.plotArea.y
                    width: chartRX.plotArea.width
                    height: chartRX.plotArea.height
                    z: 4
                    visible: width > 0 && height > 0

                    Repeater {
                        model: backend.ham_radio_bands

                        delegate: Rectangle {
                            property real parentW: backgroundBandsLayer.width
                            property real start_freq: backend.start_freq
                            property real band_span: backend.band_span

                            x: ((modelData.lo - start_freq) / band_span) * parentW
                            width: ((modelData.hi - modelData.lo) / band_span) * parentW
                            height: backgroundBandsLayer.height

                            color: Qt.rgba(0, 0, 0, 0.15)
                            visible: width > 3

                            Component.onCompleted: {
                                console.log("BAND DEBUG -> lo:", modelData.lo,
                                            "hi:", modelData.hi,
                                            "backend.start:", start_freq,
                                            "backend.span:", band_span,
                                            "Computed Width:", width)
                            }
                        }
                    }
                }

                RowLayout {
                    id: customLegend
                    anchors.top: chartContainer.top
                    anchors.right: chartContainer.right

                    // Maps offsets dynamically using the active chart plot boundaries
                    anchors.topMargin: chartRX.plotArea.y + 8
                    anchors.rightMargin: (chartContainer.width - (chartRX.plotArea.x + chartRX.plotArea.width)) + 16
                    spacing: 16
                    z: 5
                    visible: chartRX.plotArea.width > 0

                    // R-Series Label Indicator Block
                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 14
                            height: 14
                            radius: 3
                            color: seriesRColor
                        }
                        Text {
                            text: qsTr("R (Resistance)")
                            font.pointSize: 10
                            font.bold: true
                            color: AapiTheme.style.chart.axisLineColor
                        }
                    }

                    // X-Series Label Indicator Block
                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 14
                            height: 14
                            radius: 3
                            color: seriesXColor
                        }
                        Text {
                            text: qsTr("X (Reactance)")
                            font.pointSize: 10
                            font.bold: true
                            color: AapiTheme.style.chart.axisLineColor
                        }
                    }
                }

                MultiEffect {
                    anchors.fill: chartVisualBox
                    source: chartVisualBox
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

    Connections {
        target: chartRX

        function onPlotAreaChanged() {
            console.log("new RX-chart plot area: " + chartRX.plotArea.width + ":" + chartRX.plotArea.height)
            backend.handleRXPlotArea(chartRX.plotArea)
        }
    }

    Connections {
        target: btnFastRX

        function onClicked() {
            backend.handleRXScanFast()
        }
    }

    Connections {
        target: btnSlowRX

        function onClicked() {
            backend.handleRXScanSlow()
        }
    }

    Connections {
        target: btnSnapshotRX

        function onClicked() {
            chartRX.grabToImage(function(result) {
                backend.handleRXSnapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Component.onCompleted: {
        backend.handleRXChartSetup(chartRXseriesR, chartRXseriesX, chartRXaxisX, chartRXaxisY)
    }

    AapiMessageBox {
        id: snapshotDialog
        caption: qsTr("New Snapshot")
        iconType: "exclamation"
        detailedText: ""
        standardButtons: Dialog.Ok
        scale: 1

        function show() {
            messageText = qsTr("RX snapshot taken into:\n") + aapi.last_snapshot
            open()
        }
    }

    function enableControls(enable) {
        btnFastRX.enabled = enable
        btnSlowRX.enabled = enable
        btnSnapshotRX.enabled = enable
    }
}