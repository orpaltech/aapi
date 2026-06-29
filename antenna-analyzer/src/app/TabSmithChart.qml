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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi


Item {
    id: tabSmithChart
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    readonly property color legendBkgndColor: AapiTheme.style.smith.legendBkgndColor
    readonly property color legendBorderColor: AapiTheme.style.smith.legendBorderColor
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    Row {
        id: row1
        width: 1280
        height: 620
        transformOrigin: Item.TopLeft

        Column {
            id: col1
            width: 140
            height: 620
            topPadding: 12
            transformOrigin: Item.TopLeft
            spacing: 12
            anchors {
                margins: 5
            }

            Button {
                id: btnFastSmith
                text: qsTr("Fast Smith")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6
                rightPadding: 8
                leftPadding: 8
                font.bold: true
                transformOrigin: Item.TopLeft
                font.pointSize: 14

                DesignEffect {
                    effects: [ DesignDropShadow { } ]
                }
            }

            Button {
                id: btnSlowSmith
                text: qsTr("Slow Smith")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6
                rightPadding: 8
                leftPadding: 8
                font.bold: true
                transformOrigin: Item.TopLeft
                font.pointSize: 14

                DesignEffect {
                    effects: [ DesignDropShadow { } ]
                }
            }

            Button {
                id: btnSnapshot
                text: qsTr("Snapshot")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6
                font.bold: true
                rightPadding: 8
                leftPadding: 8
                transformOrigin: Item.TopLeft
                font.pointSize: 14

                DesignEffect {
                    effects: [ DesignDropShadow { } ]
                }
            }

            Button {
                id: btnResetView
                text: qsTr("Reset View")
                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.leftMargin: 6
                rightPadding: 8; leftPadding: 8;
                font.bold: true;
                transformOrigin: Item.TopLeft;
                font.pointSize: 14

                // Active only when the user has manipulated the chart size
                enabled: chartContainer.scale > 1.0

                DesignEffect {
                    effects: [ DesignDropShadow { } ]
                }

                onClicked: {
                    // Force the physical scaling back to exactly 100%
                   chartContainer.scale = 1.0;

                   // Erase the hidden X/Y layout shifts caused by the Pinch gesture
                   chartContainer.x = 0;
                   chartContainer.y = 0;

                   // Clear out any Flickable scroll coordinates completely
                   chartViewport.contentX = 0;
                   chartViewport.contentY = 0;

                   // Lock physics constraint boundaries
                   chartViewport.returnToBounds();
                }
            }
        }

        Column {
            id: col2
            width: 1140
            height: 620
            transformOrigin: Item.TopLeft

            Item {
                width: 1140
                height: 620

                Flickable {
                    id: chartViewport
                    anchors.fill: parent
                    clip: true

                    contentWidth: chartContainer.width * chartContainer.scale
                    contentHeight: chartContainer.height * chartContainer.scale
                    interactive: chartContainer.scale > 1.0

                    Item {
                        id: chartContainer
                        width: 1140
                        height: 620
                        transformOrigin: Item.Center

                        SmithChart {
                            id: chartSmith
                            anchors.fill: parent

                            // Sets the canvas background caching layer to maximum antialiasing quality
                            antialiasing: true
                            smooth: true
                        }

                        PinchHandler {
                            id: pinchHandler
                            minimumScale: 1.0
                            maximumScale: 4.0
                            minimumRotation: 0
                            maximumRotation: 0
                        }
                    }
                }

                // A styled Button container bypasses layout loop crashes and draws backgrounds cleanly
                Button {
                    id: floatingHud
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: 15
                    anchors.rightMargin: 15

                    // Fixed safe dimensions to hold the larger fonts comfortably
                    width: 220
                    height: 105

                    // Disable clicks so it acts strictly as a static visual overlay pane
                    enabled: false
                    checkable: false

                    // Toggles layout presence automatically as trace data populates the grid array
                    visible: true //chartSmith.gammaVals && chartSmith.gammaVals.length > 0

                    // Handled via your project's working effects pipeline
                    DesignEffect {
                        effects: [ DesignInnerShadow { } ]
                    }

                    background: Canvas {
                        anchors.fill: parent
                        onPaint: {
                            var ctx = getContext("2d");
                            ctx.reset();

                            // Draw your background fill
                            ctx.fillStyle = legendBkgndColor;
                            ctx.fillRect(0, 0, width, height);

                            // Draw a crisp border outline matching the theme style
                            ctx.strokeStyle = legendBorderColor;
                            ctx.lineWidth = 1;
                            ctx.strokeRect(0, 0, width, height);
                        }
                    }

                    // Stacks text lines vertically inside the styled container area
                    contentItem: Column {
                        spacing: 6
                        topPadding: 4
                        leftPadding: 4

                        Text {
                            text: "Freq: " + (backend ? (backend.last_measure_freq / 1000000).toFixed(3) : "0.000") + " MHz"
                            color: "white"
                            font.pointSize: 12 // Increased font size
                            font.bold: true
                        }

                        Text {
                            text: "Z: " + (backend ? backend.last_rx_string : "0.0 ± j0.0") + " Ω"
                            color: chartSmith.fillColor ? chartSmith.fillColor : "#82edea"
                            font.pointSize: 11 // Increased font size
                        }

                        Text {
                            property real currentSwr: backend ? backend.last_vswr_val : 1.0
                            text: "SWR: " + currentSwr.toFixed(2)
                            color: currentSwr > 2.0 ? "#dc0e0e" : (currentSwr > 1.5 ? "#f39c12" : "#2ecc71")
                            font.pointSize: 11
                            font.bold: true
                        }


                        // Placed flat inside the Column.
                        // If scale is 1.0, it renders as an empty invisible line;
                        // when zoomed, it expands reactively.
                        /*Text {
                            text: chartContainer.scale > 1.0 ? "Zoom: " + chartContainer.scale.toFixed(1) + "x" : ""
                            color: "rgba(255, 255, 255, 0.4)"
                            font.pointSize: 11
                            font.italic: true

                            // Adjust height so it doesn't take up empty spacing layout blocks when hidden
                            height: text === "" ? 0 : implicitHeight
                            visible: text !== ""
                        }*/
                    }
                }
            }
        }
    }

    Connections {
        target: chartSmith

        function onPlotAreaChanged() {
            console.log("new Smith-chart plot area: " + chartSmith.plotArea.width + ":" + chartSmith.plotArea.height)
            backend.handleSmithPlotArea(chartSmith.plotArea)
        }
    }

    Connections {
        target: btnFastSmith

        function onClicked() {
            backend.handleSmithScanFast()
        }
    }

    Connections {
        target: btnSlowSmith

        function onClicked() {
            backend.handleSmithScanSlow()
        }
    }

    Connections {
        target: btnSnapshot

        function onClicked() {
            chartSmith.grabToImage(function(result) {
                backend.handleSmithSnapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Connections {
        target: tabSmithChart.backend
        enabled: tabSmithChart.backend !== null

        function onScanStarted(numSteps) {
            btnFastSmith.enabled = false
            btnSlowSmith.enabled = false
        }

        function onScanComplete() {
            btnFastSmith.enabled = true
            btnSlowSmith.enabled = true
        }
    }


    Component.onCompleted: {
        backend.handleSmithChartSetup(chartSmith)
    }

    Dialog {
        id: snapshotDialog
        title: qsTr("New Snapshot")
        modal: true
        x: (Screen.width - snapshotDialog.width) / 2
        y: (Screen.height - snapshotDialog.height) / 2
        standardButtons: Dialog.Ok

        Label {
            id: labelMessage
            wrapMode: Text.WordWrap
        }

        function show() {
            labelMessage.text = qsTr("Smith snapshot taken into: ") + aapi.last_snapshot
            open()
        }
    }


    function enableControls(enable) {
        btnFastSmith.enabled = enable
        btnSlowSmith.enabled = enable
        btnSnapshot.enabled = enable
    }
}
