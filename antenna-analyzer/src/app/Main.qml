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
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem && aapi.audio_device_okay && aapi.audio_format_okay
                        sourceComponent: ViewSignalProcess {
                            id: vSignalProcess
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewConfiguration {
                            id: vConfiguration
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewPanoramicScan {
                            id: vPanoramicScan
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewMeasurement {
                            id: vMeasurement
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewGenerator {
                            id: vGenerator
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewHwCalibration {
                            id: vHwCalibration
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewOslCalibration {
                            id: vOslCalibration
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
                        }
                    }

                    Loader {
                        // keep a maximum of three pages instantiated
                        active: SwipeView.isCurrentItem
                        sourceComponent: ViewAboutApp {
                            id: vAbout
                            onViewStatusChanged: viewStatusChange()
                            onLoaded: viewLoadComplete()
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
                        font.pointSize: 19
                        width: contentItem.implicitWidth + leftPadding + rightPadding

                        DesignEffect {
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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
                            effects: [
                                DesignInnerShadow {
                                }
                            ]
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

        Component.onCompleted: {
            console.log("main window loaded")

            if (!(aapi.audio_device_okay && aapi.audio_format_okay)) {
                audoErrorMsg.open()
            }
        }
    }


    function viewStatusChange(status) {
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

    function viewLoadComplete() {
        labelTitle.text = tabBarMain.itemAt(tabBarMain.currentIndex).text
    }
}
