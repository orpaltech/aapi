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
import ru.orpaltech.aapi

SwipePage {
    id: swpGenerator
    title: qsTr("Generator")
    transformOrigin: Item.TopLeft
    backend: aapi.view_generator

    // Shared layout color references matching your styles context
    readonly property color txtColor: AapiTheme.style.titleFontColor
    readonly property color accentColor: AapiTheme.style.accentColor
    readonly property color panelBackColor: AapiTheme.style.backdropPanelBackColor
    readonly property color panelBorderColor: AapiTheme.style.backdropPanelBorderColor

    Column {
        id: col1
        // Indent the entire layout frame away from the top-left screen corner
        x: 16
        y: 16
        // Restrict maximum width so children don't hit the right display edge
        // 1280 total width - 16px left margin - 16px right margin = 1248px
        width: 1248
        // 656 total height - 16px top margin - 16px bottom margin = 624px
        height: 624

        transformOrigin: Item.TopLeft
        spacing: 12 // Smooth padding between your sections

        // SECTION 1: Top Status Header & Real-time Signal Validation Diagnostics
        RowLayout {
            // Span across the newly restricted 1248px width limit perfectly
            width: parent.width
            height: 35

            Label {
                text: qsTr("Continuous Wave RF Generator")
                font.pixelSize: 20
                font.bold: true
                color: AapiTheme.style.titleFontColor
            }

            Item { Layout.fillWidth: true } // Clean built-in QML spacer block

            // Visual Signal Status Badge matching Yury's "Signal OK / No Signal" flag
            Rectangle {
                id: rectSignalStatus
                width: 120
                height: 30
                radius: 4
                color: (backend && backend.measuredVmv > 1.0) ? "#2E7D32" : "#C62828"
                Layout.rightMargin: 16 // Added right padding so it isn't flush with the edge

                Label {
                    anchors.centerIn: parent
                    text: (backend && backend.measuredVmv > 1.0) ? qsTr("SIGNAL OK") : qsTr("NO SIGNAL")
                    color: "white"
                    font.bold: true
                    font.pointSize: 10
                }
            }
        }

        // =========================================================================
        // FREQUENCY TUNING NAVIGATION BLOCK (Matched exactly with your row1)
        // =========================================================================
        Item {
            id: row1
            width: parent.width // Automatically inherits the uniform 1248px span boundary
            height: 100
            z: 1
            transformOrigin: Item.TopLeft

            RowLayout {
                id: rowLayout
                width: 1200
                anchors.centerIn: parent // Safe to use now because parent is an Item, not a Row!
                transformOrigin: Item.TopLeft

                Label {
                    id: labelFreq
                    text: qsTr("Frequency:")
                    horizontalAlignment: Text.AlignLeft
                    font.bold: true
                    font.pointSize: 20
                    color: txtColor
                    transformOrigin: Item.TopLeft
                }

                Button {
                    id: btnDown3; text: "<<<"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_DOWN_LARGE)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }

                Button {
                    id: btnDown2; text: "<<"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_DOWN_MEDIUM)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }

                Button {
                    id: btnDown1; text: "<"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_DOWN_SMALL)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }

                TextField {
                    id: textFreq
                    text: getFormattedFrequency()
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    placeholderText: qsTr("Frequency")
                    transformOrigin: Item.TopLeft
                    font { pointSize: 24; family: "Monospace"; bold: true }
                    color: accentColor
                    background: Rectangle {
                        color: panelBackColor; border.color: panelBorderColor; radius: 4
                    }

                    inputMethodHints: Qt.ImhDigitsOnly

                    onActiveFocusChanged: {
                        if (backend) {
                            if (activeFocus) {
                                text = backend.frequency;
                            } else {
                                var typedKHz = parseInt(textFreq.text.replace(/[^0-9]/g, ""));
                                if (!Number.isNaN(typedKHz)) {
                                    backend.handleDirectFreqInput(typedKHz);
                                }
                                text = Qt.binding(getFormattedFrequency);
                            }
                        }
                    }

                    onEditingFinished: { focus = false; }
                    DesignEffect { effects: [ DesignInnerShadow { }, DesignDropShadow { } ] }
                }

                Button {
                    id: btnUp1; text: ">"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_UP_SMALL)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }

                Button {
                    id: btnUp2; text: ">>"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_UP_MEDIUM)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }

                Button {
                    id: btnUp3; text: ">>>"; Layout.preferredWidth: 100
                    font.bold: false; font.pointSize: 24; transformOrigin: Item.TopLeft; autoRepeat: true
                    onClicked: {
                        if (textFreq.activeFocus) textFreq.focus = false;
                        if (backend) backend.handleTuneFrequency(GeneratorViewBackend.TUNE_UP_LARGE)
                    }
                    DesignEffect { effects: [ DesignDropShadow { } ] }
                }
            }
        }

        // =========================================================================
        // SECTION 4: Your original Live RF Metrics Grid (Now fully visible)
        // =========================================================================
        Rectangle {
            id: rectMetricsDisplay
            width: parent.width // Clean uniform 1248px span mapping

            // Dynamic Height Equation: Total Column height (656) minus margins, spacings,
            // Header height (35), and Tuning Row height (100).
            height: parent.height - 35 - 100 - (col1.spacing * 2) // Exactly 497 pixels

            color: panelBackColor
            border.color: panelBorderColor
            radius: 6

            GridLayout {
                anchors.fill: parent
                anchors.margins: 14
                columns: 2
                rowSpacing: 10
                columnSpacing: 20
                Label {
                    text: qsTr("Raw Receiver Channels:"); font.bold: true; font.pointSize: 14; color: txtColor
                }
                Label {
                    text: backend ? ("Vi: " + backend.measuredImv.toFixed(1) + " mV | Vv: " + backend.measuredVmv.toFixed(1) + " mV") : "Vi: 0.0 mV | Vv: 0.0 mV";
                    font.family: "Monospace";
                    font.pointSize: 14;
                    color: txtColor
                }
                Label {
                    text: qsTr("Raw Phase Difference:");
                    font.bold: true;
                    font.pointSize: 14;
                    color: txtColor
                }
                Label {
                    text: backend ? (backend.measuredPhaseDeg.toFixed(1) + "°") : "0.0°"
                    font.family: "Monospace";
                    font.pointSize: 14;
                    color: txtColor
                }
                Label {
                    text: qsTr("Raw Impedance Vector:");
                    font.bold: true;
                    font.pointSize: 14;
                    color: txtColor
                }
                Label {
                    text: backend ? (backend.rawR.toFixed(1) + (backend.rawX >= 0 ? " + j" : " - j") + Math.abs(backend.rawX).toFixed(1) + " Ω") : "0.0 + j0.0 Ω";
                    font.family: "Monospace";
                    font.pointSize: 14;
                    color: "orange"
                }
                Label {
                    text: qsTr("Calibrated Load Z (with OSL):");
                    font.bold: true;
                    font.pointSize: 14;
                    color: txtColor
                }
                Label {
                    text: backend ? (backend.oslR.toFixed(1) + (backend.oslX >= 0 ? " + j" : " - j") + Math.abs(backend.oslX).toFixed(1) + " Ω") : "0.0 + j0.0 Ω";
                    font.family: "Monospace";
                    font.bold: true;
                    font.pointSize: 14;
                    color: "#4CAF50"
                }
            }
            DesignEffect { effects: [ DesignInnerShadow { } ] }
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
        ignoreUnknownSignals: true

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

    function getFormattedFrequency() {
        return backend ? (backend.frequency + " KHz") : "14000 KHz";
    }
}