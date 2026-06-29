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
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.Studio.DesignEffects
import aapi


Window {
    id: wndMain
    width: 720
    height: 1280
    visible: true
    flags: Qt.Window
    contentOrientation: Qt.LandscapeOrientation
    title: qsTr("ORPALTECH AA-PI")

    Rotation {
        id: rotationMain
        angle: 90
        origin.x: viewMain.height/2
        origin.y: viewMain.height/2
    }

    Overlay.overlay.transform: rotationMain

    Item {
        id: viewMain
        width: 1280
        height: 720

        transform: rotationMain

        ColumnLayout {
            id: column1
            anchors.fill: parent
            spacing: 0

            RowLayout {
                id: row1
                spacing: 0

                Rectangle {
                    id: rcStatusBar
                    width: 1280
                    height: 28
                    color: AapiTheme.style.headerBackColor
                    border.width: 0

                    Label {
                        id: labelTitle
                        width: 400
                        text: qsTr("DSP View")
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.leftMargin: 8
                        font.italic: false
                        font.bold: true
                        font.pointSize: 18
                        color: AapiTheme.style.titleFontColor
                        transformOrigin: Item.TopLeft

                        DesignEffect {
                            effects: [
                                DesignDropShadow {
                                }
                            ]
                        }
                    }

                    BatteryIndicator {
                        id: batteryIcon
                        width: 32
                        height: 25
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.rightMargin: 13
                        anchors.topMargin: 2
                    }

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            }
                        ]
                    }
                }
            }

            RowLayout {
                id: row2
                spacing: 0

                SwipeView {
                    id: svMainViews
                    width: 1280
                    height: 656
                    currentIndex: tabBarMain.currentIndex


                    Loader {
                        id: ldrSignalProcess
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem && aapi.audio_device_okay && aapi.audio_format_okay
                        sourceComponent: ViewSignalProcess {
                            id: vSignalProcess
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrSignalProcess.item
                            property: "isPageActive"
                            value: ldrSignalProcess.SwipeView.isCurrentItem
                            when: ldrSignalProcess.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrConfiguration
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewConfiguration {
                            id: vConfiguration
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrConfiguration.item
                            property: "isPageActive"
                            value: ldrConfiguration.SwipeView.isCurrentItem
                            when: ldrConfiguration.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrPanoramicScan
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewPanoramicScan {
                            id: vPanoramicScan
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrPanoramicScan.item
                            property: "isPageActive"
                            value: ldrPanoramicScan.SwipeView.isCurrentItem
                            when: ldrPanoramicScan.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrMeasurement
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewMeasurement {
                            id: vMeasurement
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrMeasurement.item
                            property: "isPageActive"
                            value: ldrMeasurement.SwipeView.isCurrentItem
                            when: ldrMeasurement.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrGenerator
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewGenerator {
                            id: vGenerator
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrGenerator.item
                            property: "isPageActive"
                            value: ldrGenerator.SwipeView.isCurrentItem
                            when: ldrGenerator.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrHwCalibration
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewHwCalibration {
                            id: vHwCalibration
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrHwCalibration.item
                            property: "isPageActive"
                            value: ldrHwCalibration.SwipeView.isCurrentItem
                            when: ldrHwCalibration.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrOslCalibration
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewOslCalibration {
                            id: vOslCalibration
                            isPageActive: SwipeView.isCurrentItem
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrOslCalibration.item
                            property: "isPageActive"
                            value: ldrOslCalibration.SwipeView.isCurrentItem
                            when: ldrOslCalibration.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    Loader {
                        id: ldrAbout
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewAboutApp {
                            id: vAbout
                            isPageActive: SwipeView.isCurrentItem
                            onViewStatusChanged: onViewPageStatusChange()
                            onLoaded: onViewPageLoadComplete()
                        }

                        Binding {
                            target: ldrAbout.item
                            property: "isPageActive"
                            value: ldrAbout.SwipeView.isCurrentItem
                            when: ldrAbout.status === Loader.Ready // Only runs when item exists
                        }
                    }

                    onCurrentIndexChanged: {
                        console.log("SwipeView index changed: " + currentIndex)
                    }
                }
            }

            RowLayout {
                id: row3
                spacing: 0

                TabBar {
                    id: tabBarMain
                    currentIndex: svMainViews.currentIndex
                    font.pointSize: 18
                    position: TabBar.Footer
                    contentHeight: 36
                    contentWidth: 1280

                    TabButton {
                        id: btnViewDsp
                        text: qsTr("DSP View")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        checked: true
                        rightPadding: 6
                        leftPadding: 6
                        font {
                            pointSize: 19
                        }
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewConfig
                        text: qsTr("Configuration")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 19
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewPanScan
                        text: qsTr("Panoramic Scan")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        transformOrigin: Item.TopLeft
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 18
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewMeasure
                        text: qsTr("Measurement")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        transformOrigin: Item.TopLeft
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 18
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewGen
                        text: qsTr("Generator")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 19
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewHwCal
                        text: qsTr("HW Calibration")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 18
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewOslCal
                        text: qsTr("OSL Calibration")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 18
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }
                    TabButton {
                        id: btnViewAbout
                        text: qsTr("About")
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        rightPadding: 6
                        leftPadding: 6
                        font.pointSize: 19
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [ DesignInnerShadow { } ]
                        }
                    }

                    onCurrentIndexChanged: {
                        console.log("tab bar index changed: " + currentIndex)
                    }
                }
            }

            AapiMessageBox {
                id: audoErrorMsg
                caption: "Error"
                iconType: "error"
                messageText: qsTr("Unable to initialize signal processing view:")
                detailedText: AapiTheme.msgs.error(aapi.warning)
                standardButtons: Dialog.Ok

                onClosed: {
                    tabBarMain.currentIndex = 1
                }
            }
        }

        // --- GLOBAL VIRTUAL KEYBOARD PANEL CONTAINER ---
        // Placed outside column1 layout loop but inside viewMain to inherit orientation
        InputPanel {
            id: inputPanel
            z: 999
            anchors.left: parent.left
            anchors.right: parent.right
            scale: 0.84
            state: "hidden"

            states: [
                State {
                    name: "hidden"
                    when: !Qt.inputMethod.visible
                    PropertyChanges {
                        target: inputPanel
                        // Shoves it completely below active row lines
                        y: viewMain.height
                        opacity: 0
                    }
                },

                State {
                    name: "visible"
                    when: Qt.inputMethod.visible
                    PropertyChanges {
                        target: inputPanel
                        // Pulls the bounding box up by its full unscaled height,
                        // then pushes it down by half of the dead-zone padding.
                        y: viewMain.height - inputPanel.height + ((inputPanel.height * (1 - inputPanel.scale)) / 2)
                        opacity: 1
                    }
                }
            ]

            transitions: Transition {
                from: "hidden"; to: "visible"
                reversible: true

                ParallelAnimation {
                    NumberAnimation {
                        properties: "y"
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        property: "opacity"
                        duration: 200
                    }
                }
            }
        }

        Component.onCompleted: {
            console.log("main window loaded")

            if (!(aapi.audio_device_okay && aapi.audio_format_okay)) {
                audoErrorMsg.open()
            }
        }
    }


    function onViewPageStatusChange(status) {
        switch (status) {
        case ViewBackend.VS_IDLE:
            svMainViews.interactive = true;
            tabBarMain.enabled = true;
            break;
        case ViewBackend.VS_BUSY:
            svMainViews.interactive = false;
            tabBarMain.enabled = false;
            break;
        }
    }

    function onViewPageLoadComplete() {
        labelTitle.text = tabBarMain.itemAt(tabBarMain.currentIndex).text
    }
}
