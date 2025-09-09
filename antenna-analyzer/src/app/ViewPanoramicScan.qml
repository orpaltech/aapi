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
import QtQuick.Window
import QtCharts
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpPanoramicScan
    property alias tabChartRX: tabChartRX
    title: "Panoramic Scan"
    transformOrigin: Item.TopLeft
    contentHeight: 656
    contentWidth: 1280
    backend: aapi.view_panoramic_scan

    Column {
        id: column1
        transformOrigin: Item.TopLeft

        Row {
            id: row1
            width: 1280
            height: 620
            transformOrigin: Item.TopLeft

            StackLayout {
                id: tabsCharts
                anchors.fill: parent
                transformOrigin: Item.TopLeft
                anchors {
                    top: tabBar.bottom
                }
                currentIndex: tabBar.currentIndex

                TabVswrChart {
                    id: tabChartVSWR
                }

                TabRxChart {
                    id: tabChartRX
                    transformOrigin: Item.TopLeft
                }

                TabPanVswr {
                    id: tabChartS11
                    transformOrigin: Item.TopLeft
                }

                TabSmithChart {
                    id: tabChartSmith
                    transformOrigin: Item.TopLeft
                }

                Connections {
                    target: tabsCharts
                    onCurrentIndexChanged: {
                        backend.tab_changed(tabsCharts.currentIndex)
                    }
                }
            }
        }

        Row {
            id: row2
            width: 1280
            height: 36
            transformOrigin: Item.TopLeft

            TabBar {
                id: tabBar
                position: TabBar.Footer
                anchors.fill: parent
                contentHeight: 36
                contentWidth: 1280
                transformOrigin: Item.TopLeft
                font.pointSize: 18

                TabButton {
                    text: "VSWR Chart"
                    anchors.top: parent.top
                    rightPadding: 6
                    leftPadding: 6
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }

                TabButton {
                    text: "RX Chart"
                    anchors.top: parent.top
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }

                TabButton {
                    text: "S11 Chart"
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }

                TabButton {
                    text: "Smith Chart"
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }
            }
        }
    }

    Dialog {
        id: scanError
        title: "Scan Error"
        modal: true
        font.pointSize: 10
        x: (Screen.width - scanError.width)/2
        y: (Screen.height - scanError.height)/2
        standardButtons: Dialog.Ok

        Label {
            id: labelScanError
        }

        function show(message) {
            labelScanError.text = message
            open()
        }
    }

    Connections {
        target: backend
        onScanStarted: {
            enableControls(false)
        }

        onScanComplete: {
            enableControls(true)
        }

        onScanError: {
            console.log("Scan error: " + message)
            scanError.show("Error occurred during scan: " + message)
            enableControls(true)
        }

        onScanNoSignal: {
            scanError.show("Low signal. Please, check hardware.")
            enableControls(true)
        }
    }


    onLoaded: {
        for (var i=0; i< tabsCharts.children.length; i++) {
            var tab = tabsCharts.children[i];
            if (tab.loaded)
                tab.loaded();
        }
    }

    /* ------------------------------------------------------------------- */
    /* The code below will handle state of the controls (enabled/disabled)
    /* ------------------------------------------------------------------- */
    function enableControls(enable) {
        for (var i=0; i< tabsCharts.children.length; i++) {
            var tab = tabsCharts.children[i];
            if (tab.enableControls)
                tab.enableControls(enable);
        }
    }
}
