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
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpHWCal
    backend: aapi.view_hw_calibration

    // Visual Anchor Highlights
    readonly property color textLabelColor: AapiTheme.style.textLabelColor
    readonly property color accentColor: AapiTheme.style.accentColor
    readonly property color panelBackColor: AapiTheme.style.backdropPanelBackColor
    readonly property color panelBorderColor: AapiTheme.style.backdropPanelBorderColor

    ColumnLayout {
        id: layoutHWCal
        spacing: 20
        anchors {
            leftMargin: 40
            topMargin: 20
            rightMargin: 40
            bottomMargin: 20
            fill: parent
        }

        Label {
            id: labelHWCal
            text: qsTr("Set PCB jumper to HW Calibration position and press Start button")
            wrapMode: Text.WordWrap
            font {
                pointSize: 16
                weight: Font.Medium
            }
            color: textLabelColor
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressCtrl {
            id: progressHWCal
            height: 36
            Layout.fillWidth: true
            // Binds dynamically to your theme architecture variables
            color: AapiTheme.style.progressGradientColor1
            secondColor: AapiTheme.style.progressGradientColor2
            value: 0
        }

        Rectangle {
            id: frequencyCard
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width * 0.8
            height: 54
            color: panelBackColor
            border.color: panelBorderColor
            border.width: 1
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20

                Label {
                    text: qsTr("Scanning Frequency")
                    font.pointSize: 11
                    font.bold: true
                    color: textLabelColor
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                Label {
                    id: labelCurrentFreq
                    text: "0.000 MHz"
                    font {
                        pointSize: 16;
                        bold: true;
                        family: "Monospace"
                    }
                    color: accentColor
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }

        // Enhanced contrast layout featuring clean backdrop panels
        RowLayout {
            id: metricsLayout
            spacing: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width * 0.8

            // Left Display Box: Magnitude Metrics
            Rectangle {
                Layout.fillWidth: true
                height: 64
                color: panelBackColor
                border.color: panelBorderColor
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 2
                    Label {
                        text: qsTr("Magnitude Ratio")
                        font.pointSize: 11
                        font.bold: true
                        color: textLabelColor
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Label {
                        id: labelMagRatio
                        text: "0.0000"
                        font { pointSize: 15; bold: true; family: "Monospace" }
                        color: accentColor
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // Right Display Box: Phase Metrics
            Rectangle {
                Layout.fillWidth: true
                height: 64
                color: panelBackColor
                border.color: panelBorderColor
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 2
                    Label {
                        text: qsTr("Phase Difference")
                        font.pointSize: 11
                        font.bold: true
                        color: textLabelColor
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Label {
                        id: labelPhaseDiff
                        text: "0.0°"
                        font { pointSize: 15; bold: true; family: "Monospace" }
                        color: accentColor
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }

        TwoStateButtonCtrl {
            id: buttonHWCal
            offText: qsTr("Start Calibration")
            onText: qsTr("Stop Calibration")
            font {
                pointSize: 18
            }
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter

            function onStart() {
                console.log("HW Cal: onStart Initiated")
                if (backend) {
                    return backend.handleStartScan() === 0
                }
                return false
            }

            function onStop() {
                console.log("HW Cal: onStop Interrupted")
                if (backend) {
                    backend.handleCancelScan()
                }
            }

            DesignEffect {
                effects: [ DesignDropShadow { } ]
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

    // This event runs automatically when swiped into focus
    onActivated: {
        console.log("is now ACTIVE")
    }

    // This event runs automatically when swiped out of focus or destroyed
    onDeactivated: {
        console.log("is now INACTIVE")

        progressHWCal.value = 0
        if (typeof buttonHWCal !== "undefined") {
            buttonHWCal.setOff()
        }
    }

    Connections {
        target: backend
        ignoreUnknownSignals: true

        onScanProgress: (step, total, freq, mag_ratio, phase_diff) => {
            progressHWCal.minimum = 0
            progressHWCal.maximum = total
            progressHWCal.value = step

            // Format frequency from C++ raw Hz directly up to readable MHz
            var currentMHz = freq / 1000000.0
            labelCurrentFreq.text = currentMHz.toFixed(3) + " MHz"

            // Format dynamic C++ floats directly to your screen text blocks
            labelMagRatio.text = mag_ratio.toFixed(4)

            // Convert phase from raw radians up to human-readable degrees
            var phaseDegrees = (phase_diff * 180.0) / Math.PI
            labelPhaseDiff.text = phaseDegrees.toFixed(1) + "°"

            if (step === total) {
                buttonHWCal.setOff()
            }
        }

        onScanNoSignal: {
            scanError.show(qsTr("Low signal level detected. Please inspect physical hardware lines."))
            buttonHWCal.setOff()
        }
    }
}
