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
    id: tabS11ChartRoot
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    readonly property string seriesColor: "#1ea8dc" // A clean, bright cyan/ice blue line
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    ColumnLayout {
        id: col1
        anchors.fill: parent

        // Toolstrip Row Container
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
                    id: btnFastS11
                    text: qsTr("Fast S11")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14
                    DesignEffect { effects: [ DesignDropShadow {} ] }
                }

                Button {
                    id: btnSlowS11
                    text: qsTr("Slow S11")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14
                    DesignEffect { effects: [ DesignDropShadow {} ] }
                }

                Button {
                    id: btnSnapshot
                    text: qsTr("Snapshot")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    font.pointSize: 14
                    DesignEffect { effects: [ DesignDropShadow {} ] }
                }

                Item { Layout.fillWidth: true } // Trailing Spacer
            }
        }

        // Main Graph Screen Content Window
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
                        id: chartS11
                        anchors.fill: parent
                        antialiasing: true
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
                            id: axisS11_X
                        }

                        axisY: ValueAxis {
                            id: axisS11_Y
                        }

                        LineSeries {
                            id: chartS11series
                            width: 2.2
                            color: seriesColor
                        }
                    }
                }

                // Hardware-Accelerated Ham Bands Overlay
                Item {
                    id: backgroundBandsLayer
                    x: chartS11.plotArea.x
                    y: chartS11.plotArea.y
                    width: chartS11.plotArea.width
                    height: chartS11.plotArea.height
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

                // Synced Descriptive Trace Legend Overlay
                RowLayout {
                    id: customLegendS11
                    anchors.top: chartContainer.top
                    anchors.right: chartContainer.right
                    anchors.topMargin: chartS11.plotArea.y + 8
                    anchors.rightMargin: (chartContainer.width - (chartS11.plotArea.x + chartS11.plotArea.width)) + 16
                    spacing: 16
                    z: 5
                    visible: chartS11.plotArea.width > 0

                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 14
                            height: 14
                            radius: 3
                            color: seriesColor
                        }
                        Text {
                            text: qsTr("S11 Log Mag (Return Loss)")
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
        target: chartS11
        function onPlotAreaChanged() {
            backend.handleS11PlotArea(chartS11.plotArea)
        }
    }

    // Map your scan actions directly into your existing fast/slow handlers
    Connections {
        target: btnFastS11
        function onClicked() {
            backend.handleS11ScanFast()
        }
    }

    Connections {
        target: btnSlowS11
        function onClicked() {
            backend.handleS11ScanSlow()
        }
    }

    Connections {
        target: btnSnapshot
        function onClicked() {
            chartS11.grabToImage(function(result) {
                backend.handleS11Snapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Component.onCompleted: {
        backend.handleS11ChartSetup(chartS11series, axisS11_X, axisS11_Y)
    }

    AapiMessageBox {
        id: snapshotDialog
        caption: qsTr("New Snapshot")
        iconType: "exclamation"
        detailedText: ""
        standardButtons: Dialog.Ok
        scale: 1

        function show() {
            messageText = qsTr("S11 snapshot taken into:\n") + aapi.last_snapshot
            open()
        }
    }

    function enableControls(enable) {
        btnFastS11.enabled = enable
        btnSlowS11.enabled = enable
        btnSnapshot.enabled = enable
    }
}