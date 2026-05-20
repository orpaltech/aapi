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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi


Item {
    id: item1
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
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
                    effects: [
                        DesignDropShadow {
                        }
                    ]
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
                    effects: [
                        DesignDropShadow {
                        }
                    ]
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
                    effects: [
                        DesignDropShadow {
                        }
                    ]
                }
            }
        }

        Column {
            id: col2
            width: 1140
            height: 620
            transformOrigin: Item.TopLeft

            SmithChart {
                id: chartSmith
                anchors.fill: parent
                transformOrigin: Item.TopLeft
                anchors {
                    left: col1.right
                    margins: 0
                }
            }
        }
    }

    Connections {
        target: chartSmith
        onPlotAreaChanged: {
            console.log("new Smith-chart plot area: " + chartSmith.plotArea.width + ":" + chartSmith.plotArea.height)
            backend.smith_plot_area(chartSmith.plotArea)
        }
    }

    Connections {
        target: btnFastSmith
        onClicked: {
            backend.smith_fast_scan()
        }
    }

    Connections {
        target: btnSlowSmith
        onClicked: {
            backend.smith_slow_scan()
        }
    }

    Connections {
        target: btnSnapshot
        onClicked: {
            /*chartRX.grabToImage(function(result) {
                backend.rx_snapshot(result)
                snapshotDialog.show()
            });*/
        }
    }


    Component.onCompleted: {
        backend.smith_setup(chartSmith)
    }

    function enableControls(enable) {
        btnFastSmith.enabled = enable
        btnSlowSmith.enabled = enable
        btnSnapshot.enabled = enable
    }
}
