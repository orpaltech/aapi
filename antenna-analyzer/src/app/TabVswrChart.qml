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
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi


Item {
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft


    Column {
        id: col1
        anchors.fill: parent
        transformOrigin: Item.TopLeft

        Row {
            id: row1
            height: 36
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            transformOrigin: Item.TopLeft

            Row {
                id: rowButtons
                spacing: 8
                anchors {
                    margins: 5
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                width: parent.width
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.topMargin: 2
                transformOrigin: Item.TopLeft

                Button {
                    id: btnFastVSWR
                    text: qsTr("Fast VSWR")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    transformOrigin: Item.TopLeft
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnSlowVSWR
                    text: qsTr("Slow VSWR")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    transformOrigin: Item.TopLeft
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnSnapshot
                    text: qsTr("Snapshot")
                    font.bold: true
                    rightPadding: 8
                    leftPadding: 8
                    transformOrigin: Item.TopLeft
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }
            }
        }

        Row {
            id: row2
            height: 584
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            transformOrigin: Item.TopLeft

            ChartView {
                id: chartVSWR
                anchors.fill: parent
                transformOrigin: Item.TopLeft
                anchors {
                    top: rowButtons.bottom
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

                axes: [
                    ValueAxis {
                        id: axisVSWR_X
                        labelsFont.pointSize: 6
                    },
                    ValueAxis {
                        id: axisVSWR_Y
                        labelsFont.pointSize: 6
                    }
                ]

                SplineSeries {
                    id: chartVSWRseries
                    name: "VSWR"
                    axisX: axisVSWR_X
                    axisY: axisVSWR_Y
                    color: "#dc1e1e"
                }
            }
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
        target: btnFastVSWR
        onClicked: {
            backend.vswr_fast_scan()
        }
    }

    Connections {
        target: btnSlowVSWR
        onClicked: {
            backend.vswr_slow_scan()
        }
    }

    Connections {
        target: btnSnapshot
        onClicked: {
            chartVSWR.grabToImage(function(result) {
                backend.vswr_snapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Component.onCompleted: {
        backend.vswr_setup(chartVSWRseries)
    }

    Dialog {
        id: snapshotDialog
        title: qsTr("New Snapshot")
        modal: true
        x: (Screen.width - snapshotDialog.width)/2
        y: (Screen.height - snapshotDialog.height)/2
        standardButtons: Dialog.Ok

        Label {
            id: labelMessage
            wrapMode: Text.WordWrap
        }

        function show() {
            labelMessage.text = qsTr("VSWR snapshot taken into: ") + analyzer.lastSnapshot
            open()
        }
    }




    function enableControls(enable) {
        btnFastVSWR.enabled = enable
        btnSlowVSWR.enabled = enable
        btnSnapshot.enabled = enable
    }
}
