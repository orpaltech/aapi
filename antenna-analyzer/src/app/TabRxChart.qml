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
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi


Item {
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    Column {
        id: column1
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
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.topMargin: 2
                transformOrigin: Item.TopLeft
                spacing: 8
                anchors {
                    margins: 5
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }

                Button {
                    id: btnFastRX
                    text: qsTr("Fast RX")
                    transformOrigin: Item.TopLeft
                    rightPadding: 10
                    leftPadding: 10
                    font.bold: true
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnSlowRX
                    text: qsTr("Slow RX")
                    transformOrigin: Item.TopLeft
                    rightPadding: 10
                    leftPadding: 10
                    font.bold: true
                    font.pointSize: 14

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnSnapshotRX
                    text: qsTr("Snapshot")
                    transformOrigin: Item.TopLeft
                    rightPadding: 10
                    leftPadding: 10
                    highlighted: false
                    font.bold: true
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
                id: chartRX
                anchors.fill: parent
                transformOrigin: Item.TopLeft
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
        target: btnFastRX
        onClicked: {
            backend.rx_fast_scan()
        }
    }

    Connections {
        target: btnSlowRX
        onClicked: {
            backend.rx_slow_scan()
        }
    }

    Connections {
        target: btnSnapshotRX
        onClicked: {
            chartRX.grabToImage(function(result) {
                backend.rx_snapshot(result)
                snapshotDialog.show()
            });
        }
    }

    Component.onCompleted: {
        backend.rx_setup(chartRXseriesR, chartRXseriesX)
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
            labelMessage.text = qsTr("RX snapshot taken into: ") + analyzer.lastSnapshot
            open()
        }
    }


    function enableControls(enable) {
        btnFastRX.enabled = enable
        btnSlowRX.enabled = enable
        btnSnapshotRX.enabled = enable
    }
}
