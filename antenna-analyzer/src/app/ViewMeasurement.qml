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
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpMeasurement
    title: qsTr("Measurement")
    backend: aapi.view_measurement


    // High-Contrast Dark Laboratory Theme Properties
    readonly property color txtColor: AapiTheme.style.textAreaColor
    readonly property color textLabelColor: AapiTheme.style.textLabelColor
    readonly property color accentColor: AapiTheme.style.accentColor

    readonly property color telemetryNormalColor: AapiTheme.style.accentColor
    readonly property color telemetryAlertColor: AapiTheme.style.telemetryAlertColor
    readonly property color telemetryDeadColor: AapiTheme.style.telemetryDeadColor

    readonly property color panelBackColor: AapiTheme.style.backdropPanelBackColor
    readonly property color panelBorderColor: AapiTheme.style.backdropPanelBorderColor

    // Green for L, Blue for C
    readonly property color inductanceColor: "#4CAF50"
    readonly property color capacitanceColor: "#2196F3"


    Column {
        id: col1
        width: 1280
        height: 656
        transformOrigin: Item.TopLeft
        spacing: 0

        // =========================================================================
        // FREQUENCY TUNING NAVIGATION BLOCK
        // =========================================================================
        Row {
            id: row1
            width: 1280
            height: 100 // Controlled layout height allocation
            z: 1
            transformOrigin: Item.TopLeft

            RowLayout {
                id: rowLayout
                width: 1200
                anchors.centerIn: parent
                transformOrigin: Item.TopLeft

                Label {
                    id: labelFreq
                    text: qsTr("Frequency:")
                    horizontalAlignment: Text.AlignLeft
                    font.bold: true
                    font.pointSize: 22
                    color: txtColor
                    transformOrigin: Item.TopLeft
                }

                Button {
                    id: btnDown3
                    text: "<<<"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_DOWN_LARGE)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }

                Button {
                    id: btnDown2
                    text: "<<"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_DOWN_MEDIUM)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }

                Button {
                    id: btnDown1
                    text: "<"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_DOWN_SMALL)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }

                TextField {
                    id: textFreq
                    text: formatFrequency()

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    placeholderText: qsTr("Frequency")
                    transformOrigin: Item.TopLeft
                    font {
                        pointSize: 24
                        family: "Monospace"
                        bold: true
                    }
                    color: accentColor
                    background: Rectangle {
                        color: panelBackColor
                        border.color: panelBorderColor
                        radius: 4
                    }

                    // --- VIRTUAL KEYBOARD CONFIGURATION ---

                    // Force the virtual keyboard to show a dedicated numeric pad
                    inputMethodHints: Qt.ImhDigitsOnly


                    onActiveFocusChanged: {
                        if (backend) {
                            if (activeFocus) {
                                // Clear out formatting text (" KHz") when user taps to edit
                                // Break binding temporarily to allow raw digit entry
                                text = backend.frequency;
                            } else {
                                // User stepped out of input box (or clicked a step button)
                                // Extract, validate, and push text changes down to C++
                                var typedKHz = parseInt(textFreq.text.replace(/[^0-9]/g, ""));
                                if (!Number.isNaN(typedKHz)) {
                                    backend.handleDirectFreqInput(typedKHz);
                                }

                                // This ensures subsequent step-button clicks instantly update this box
                                text = Qt.binding(formatFrequency);
                            }
                        }
                    }

                    // Clean up the input panel and force focus yielding when "Done" is tapped
                    onEditingFinished: {
                        focus = false;
                    }

                    DesignEffect {
                        effects: [ DesignInnerShadow { }, DesignDropShadow { } ]
                    }
                }

                Button {
                    id: btnUp1
                    text: ">"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_UP_SMALL)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }

                Button {
                    id: btnUp2
                    text: ">>"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_UP_MEDIUM)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }

                Button {
                    id: btnUp3
                    text: ">>>"
                    Layout.preferredWidth: 100
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true
                    onClicked: {
                        if (backend)
                            backend.handleTuneFrequency(MeasurementViewBackend.TUNE_UP_LARGE)
                    }
                    DesignEffect {
                        effects: [ DesignDropShadow { } ]
                    }
                }
            }
        }

        // =========================================================================
        // MEASUREMENT TELEMETRY DASHBOARD GRID
        // =========================================================================
        Pane {
            id: pane1
            width: 1280
            height: 556
            transformOrigin: Item.TopLeft

            // Remove native padding background so cards can render on your layout space seamlessly
            background: Rectangle { color: "transparent" }

            GroupBox {
                id: groupBox1
                x: 9
                y: 0
                width: 1262
                height: 520
                font {
                    underline: false
                    bold: true
                    italic: true
                    pointSize: 18
                }
                transformOrigin: Item.TopLeft
                title: qsTr("Measure Frequency Metrics")

                // Embed the modern telemetry display grid layout cleanly
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 20

                    // THE HERO CARD: VOLTAGE STANDING WAVE RATIO (VSWR)
                    Rectangle {
                        id: vswrHeroCard
                        Layout.fillWidth: true
                        Layout.preferredHeight: 180
                        color: panelBackColor
                        border {
                            color: panelBorderColor
                            width: 1
                        }
                        radius: 6

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 4

                            Label {
                                text: qsTr("VOLTAGE STANDING WAVE RATIO (VSWR)")
                                font {
                                    pointSize: 11;
                                    bold: true
                                }
                                color: textLabelColor
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                id: lblGiantSWR
                                text: "1.00"
                                font {
                                    pointSize: 72;
                                    bold: true;
                                    family: "Monospace"
                                }

                                // Color logic: Green if safe tuning, Red if high impedance mismatch
                                color: parseFloat(text) <= 1.5 ? telemetryNormalColor :
                                       (parseFloat(text) >= 3.0 ? telemetryDeadColor : telemetryAlertColor)

                                Layout.alignment: Qt.AlignHCenter

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }
                        }
                    }


                    RowLayout {
                        spacing: 20
                        Layout.fillWidth: true
                        Layout.preferredHeight: 180

                        // IMPEDANCE ANALYSIS CARD
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: panelBackColor
                            border {
                                color: panelBorderColor
                                width: 1
                            }
                            radius: 4

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 4

                                Label {
                                    text: qsTr("IMPEDANCE (Z)")
                                    font {
                                        pointSize: 11;
                                        bold: true
                                    }
                                    color: textLabelColor
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Label {
                                    id: lblImpedance
                                    text: "50.0 + j0.0 Ω"
                                    font {
                                        pointSize: 28;
                                        bold: true;
                                        family: "Monospace"
                                    }
                                    color: txtColor
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Label {
                                    id: lblComponent
                                    text: "Resonant (0.0 pF / 0.0 μH)"
                                    font {
                                        pointSize: 12;
                                        family: "Monospace"
                                    }
                                    color: textLabelColor
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }

                        // REFLECTION CARD
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: panelBackColor
                            border {
                                color: panelBorderColor
                                width: 1
                            }
                            radius: 4

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 4

                                Label {
                                    text: qsTr("RETURN LOSS & PHASE")
                                    font {
                                        pointSize: 11;
                                        bold: true
                                    }
                                    color: textLabelColor
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                Label {
                                    id: lblReturnLoss
                                    text: "0.00 dB"
                                    font {
                                        pointSize: 28;
                                        bold: true;
                                        family: "Monospace"
                                    }
                                    color: txtColor
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                // Appends the raw vector phase angle below the decibel amplitude!
                                Label {
                                    id: lblVectorPhase
                                    text: "Phase: 0.0°"
                                    font {
                                        pointSize: 14;
                                        bold: true;
                                        family: "Monospace"
                                    }
                                    color: accentColor
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    AapiMessageBox {
        id: measureError
        caption: qsTr("Error")
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
    }

    Connections {
        target: backend
        ignoreUnknownSignals: true  // Emitted continuously directly from your non-blocking C++ consumer thread pipeline pass

        // Change from an arrow function to standard function syntax
        function onMetricsMeasureFinished(vswr, r_real, x_imag, return_loss, phase_deg, component_val, is_inductive) {
            lblGiantSWR.text = vswr.toFixed(2)

            var signStr = x_imag >= 0 ? " + j" : " - j"
            lblImpedance.text = r_real.toFixed(1) + signStr + Math.abs(x_imag).toFixed(1) + " Ω"

            let comp = formatComponent(component_val, is_inductive);

            // Dynamically print matching component metrics based on sign direction
            if (Math.abs(x_imag) < 0.1) {
                lblComponent.text = "Resonant";
            } else if (is_inductive) {
                // Uses the auto-scaled value and dynamic unit (nH, µH, mH)
                lblComponent.text = "L: " + comp.valueStr + " " + comp.unitStr;
            } else {
                // Uses the auto-scaled value and dynamic unit (pF, nF, µF)
                lblComponent.text = "C: " + comp.valueStr + " " + comp.unitStr;
            }
            lblComponent.color = is_inductive ? inductanceColor : capacitanceColor;

            lblReturnLoss.text = return_loss.toFixed(2) + " dB"
            lblVectorPhase.text = "Phase: " + phase_deg.toFixed(1) + "°"
        }

        function onMetricsMeasureError(message) {
            measureError.show(message);
        }
    }

    MouseArea {
        anchors.fill: parent
        z: 99999

        // Only sits on top when the text box is active
        visible: textFreq.activeFocus

        onPressed: {
            Qt.inputMethod.hide()
            textFreq.focus = false
        }
    }

    // Local JS Formatting Helper for the L/C values
    function formatComponent(val, is_inductive) {
        if (val === 0 || Number.isNaN(val) || !Number.isFinite(val)) {
            return { valueStr: "0.00", unitStr: is_inductive ? "nH" : "pF" };
        }

        let unit = "??";
        if (is_inductive) {
            // Value comes in Henrys (H)
            if (val < 1e-6) {       val *= 1e9; unit = "nH"; }
            else if (val < 1e-3) {  val *= 1e6; unit = "µH"; }
            else {                  val *= 1e3; unit = "mH"; }
        } else {
            // Value comes in Farads (F)
            if (val < 1e-9) {       val *= 1e12; unit = "pF"; }
            else if (val < 1e-6) {  val *= 1e9;  unit = "nF"; }
            else {                  val *= 1e6;  unit = "µF"; }
        }

        return {
            valueStr: val.toFixed(2),
            unitStr: unit
        };
    }


    function formatFrequency() {
        return backend ? (backend.frequency + " KHz") : "14000 KHz";
    }
}
