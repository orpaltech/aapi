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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Studio.DesignEffects
import QtQuick.Effects
import aapi
import ru.orpaltech.aapi

Item {
    id: tabVSWRChart
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    readonly property string seriesColor: "#dc1e1e"

    ColumnLayout {
        id: col1
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
                    id: btnFastVSWR
                    text: qsTr("Fast VSWR")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                Button {
                    id: btnSlowVSWR
                    text: qsTr("Slow VSWR")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                Button {
                    id: btnSnapshot
                    text: qsTr("Snapshot")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {}
                        ]
                    }
                }

                // This invisible spacer absorbs all trailing space,
                // pushing the buttons together on the left side.
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
                        id: chartVSWR
                        anchors.fill: parent
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
                            id: axisVSWR_X
                        }
                        axisY: ValueAxis {
                            id: axisVSWR_Y
                        }

                        LineSeries {
                            id: chartVSWRseries
                            width: 2.2
                            color: seriesColor
                        }
                    }
                }

                Item {
                    id: backgroundBandsLayer
                    x: chartVSWR.plotArea.x
                    y: chartVSWR.plotArea.y
                    width: chartVSWR.plotArea.width
                    height: chartVSWR.plotArea.height
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
                    id: customLegendVSWR
                    anchors.top: chartContainer.top
                    anchors.right: chartContainer.right

                    // Aligns dynamically relative to the active plot area boundaries
                    anchors.topMargin: chartVSWR.plotArea.y + 8
                    anchors.rightMargin: (chartContainer.width - (chartVSWR.plotArea.x + chartVSWR.plotArea.width)) + 16
                    spacing: 16
                    z: 5
                    visible: chartVSWR.plotArea.width > 0

                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 14
                            height: 14
                            radius: 3
                            color: seriesColor
                        }
                        Text {
                            text: qsTr("VSWR")
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
        target: chartVSWR
        function onPlotAreaChanged() {
            console.log("new VSWR-chart plot area: " + chartVSWR.plotArea.width + ":" + chartVSWR.plotArea.height)
            backend.handleVSWRPlotArea(chartVSWR.plotArea)
        }
    }

    Connections {
        target: btnFastVSWR
        function onClicked() {
            backend.handleVSWRScanFast()
        }
    }

    Connections {
        target: btnSlowVSWR
        function onClicked() {
            backend.handleVSWRScanSlow()
        }
    }

    Connections {
        target: btnSnapshot
        function onClicked() {
            chartVSWR.grabToImage(function(result) {
                backend.handleVSWRSnapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Component.onCompleted: {
        backend.handleVSWRChartSetup(chartVSWRseries, axisVSWR_X, axisVSWR_Y)
    }

    AapiMessageBox {
        id: snapshotDialog
        caption: qsTr("New Snapshot")
        iconType: "exclamation"
        detailedText: ""
        standardButtons: Dialog.Ok
        scale: 1

        function show() {
            messageText = qsTr("VSWR snapshot taken into:\n") + aapi.last_snapshot
            open()
        }
    }

    function enableControls(enable) {
        btnFastVSWR.enabled = enable
        btnSlowVSWR.enabled = enable
        btnSnapshot.enabled = enable
    }
}