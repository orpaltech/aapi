/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

ApplicationWindow {
    visible: true
    title: "ORPAL AA (PI Edition)"

    GroupBox {
        id: groupBox
        rightPadding: 6
        leftPadding: 6
        topPadding: 28
        bottomPadding: 6
        anchors.topMargin: 2
        anchors.fill: parent

        SwipeView {
            id: swipeView
            anchors.fill: parent
            currentIndex: tabBar.currentIndex

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: DSPView {
                    id: viewDSP

                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "DSP View"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: ConfigView {
                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "Configuration"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: PanVSWRView {
                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "Panoramic Scan"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: MeasurementView {
                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "Measurements"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: Page {
                    Label {
                        text: "Generator"
                        anchors.centerIn: parent
                    }

                    Component.onCompleted: {
                        groupBox.title = "Generator"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: HWCalibrationView {
                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "HW Calibration"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: OSLCalibrationView {
                    onViewStatusChanged: viewStatusChange()
                    onLoaded: {
                        groupBox.title = "OSL Calibration"
                    }
                }
            }

            Loader {
                active: SwipeView.isCurrentItem
                sourceComponent: AboutView {
                    Component.onCompleted: {
                        groupBox.title = "About"
                    }
                }
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        font.pointSize: 12
        width: 1200
        height: (parent.height/10)
        TabButton {
            text: qsTr("DSP View")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Configuration")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Panoramic Scan")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Measurements")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Generator")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("HW Calibration")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("OSL Calibration")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("About")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
    }

    Connections {
        target: tabBar
        onCurrentIndexChanged: {
            console.log("main tabbar index changed: " + tabBar.currentIndex)
        }
    }

    function viewStatusChange(status) {
        switch (status) {
        case ViewBackend.VS_IDLE:
            swipeView.interactive = true;
            tabBar.enabled = true;
            break;
        case ViewBackend.VS_BUSY:
            swipeView.interactive = false;
            tabBar.enabled = false;
            break;
        }
    }
}
