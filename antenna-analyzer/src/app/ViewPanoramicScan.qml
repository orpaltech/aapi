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
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpPanoramicScan
    title: qsTr("Panoramic Scan")
    contentHeight: 656
    contentWidth: 1280
    transformOrigin: Item.TopLeft
    backend: aapi.view_panoramic_scan

    property alias tabChartRX: tabChartRX

    ColumnLayout {
        id: column1
        anchors.fill: parent
        spacing: 0

        Item {
            id: row1
            Layout.fillWidth: true
            Layout.fillHeight: true

            StackLayout {
                id: tabsCharts
                anchors.fill: parent
                currentIndex: tabBar.currentIndex

                TabPanScanSetup {
                    id: tabPanFreqSetup
                    transformOrigin: Item.TopLeft
                }

                TabVswrChart {
                    id: tabChartVSWR
                    transformOrigin: Item.TopLeft
                }

                TabRxChart {
                    id: tabChartRX
                    transformOrigin: Item.TopLeft
                }

                TabS11Chart {
                    id: tabChartS11
                    transformOrigin: Item.TopLeft
                }

                TabSmithChart {
                    id: tabChartSmith
                    transformOrigin: Item.TopLeft
                }

                Layout.fillWidth: false
                Layout.fillHeight: false

                Connections {
                    target: tabsCharts

                    function onCurrentIndexChanged() {
                        backend.handleTabChange(tabsCharts.currentIndex)
                    }
                }
            }
        }

        Item {
            id: row2
            Layout.fillWidth: true
            Layout.preferredHeight: 36

            TabBar {
                id: tabBar
                position: TabBar.Footer
                anchors.fill: parent
                contentHeight: 36
                contentWidth: 1280
                transformOrigin: Item.TopLeft
                font.pointSize: 18

                TabButton {
                    id: btnTabFreq
                    text: "Scan Setup"
                    rightPadding: 6
                    leftPadding: 6
                    width: implicitWidth + leftPadding + rightPadding
                    DesignEffect { effects: [ DesignInnerShadow {} ] }
                }

                TabButton {
                    id: btnTabVswr
                    text: "VSWR Chart"
                    rightPadding: 6
                    leftPadding: 6
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect { effects: [ DesignInnerShadow { } ] }
                }

                TabButton {
                    id: btnTabRx
                    text: "RX Chart"
                    rightPadding: 6
                    leftPadding: 6
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect { effects: [ DesignInnerShadow {} ] }
                }

                TabButton {
                    text: "S11 Chart"
                    rightPadding: 6
                    leftPadding: 6
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect { effects: [ DesignInnerShadow {} ] }
                }

                TabButton {
                    text: "Smith Chart"
                    width: implicitWidth + leftPadding + rightPadding

                    DesignEffect { effects: [ DesignInnerShadow {} ] }
                }
            }
        }
    }

    AapiMessageBox {
        id: scanError
        caption: qsTr("Scan Error")
        detailedText: ""
        standardButtons: Dialog.Ok

        function show(message) {
            messageText = message
            open()
        }
    }

    Connections {
        target: backend

        function onScanStarted() {
            enableControls(false)
        }

        function onScanComplete() {
            enableControls(true)
        }

        function onScanError(message) {
            console.log("Scan error: " + message)
            scanError.show("Error occurred during scan: " + message)
            enableControls(true)
        }

        function onScanNoSignal() {
            scanError.show("Low signal. Please, check hardware.")
            enableControls(true)
        }
    }

    onLoaded: {
        for (var i = 0; i < tabsCharts.children.length; i++) {
            var tab = tabsCharts.children[i];
            if (tab && tab.loaded) {
                tab.loaded();
            }
        }
    }

    function enableControls(enable) {
        for (var i = 0; i < tabsCharts.children.length; i++) {
            var tab = tabsCharts.children[i];
            if (tab && tab.enableControls) {
                tab.enableControls(enable);
            }
        }
    }
}
