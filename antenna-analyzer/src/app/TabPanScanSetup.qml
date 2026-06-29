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
import QtQml
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi

Item {
    id: tabPanScanSetupRoot
    property PanoramicScanViewBackend backend: aapi.view_panoramic_scan
    width: 1280
    height: 620
    transformOrigin: Item.TopLeft

    // High-Contrast Dark Laboratory Theme Properties (Synced from ViewMeasurement)
    readonly property color txtColor: AapiTheme.style.textAreaColor
    readonly property color textLabelColor: AapiTheme.style.textLabelColor
    readonly property color accentColor: AapiTheme.style.accentColor
    readonly property color panelBackColor: AapiTheme.style.backdropPanelBackColor
    readonly property color panelBorderColor: AapiTheme.style.backdropPanelBorderColor

    readonly property color controlBackColor: AapiTheme.style.controlBackColor
    readonly property color controlBorderColor: AapiTheme.style.controlBorderColor
    readonly property color controlFontColor: AapiTheme.style.controlFontColor


    MouseArea {
        anchors.fill: parent
        z: 99999

        // Only sits on top when the text box is active
        visible: txtFreq.activeFocus

        onPressed: {
            Qt.inputMethod.hide()
            txtFreq.focus = false
        }
    }

    Rectangle {
        id: settingsFormBox
        anchors.centerIn: parent
        width: 1200 // Expanded layout width to comfortably fit the tuning button strip
        height: 380
        color: panelBackColor
        radius: 6
        border.color: panelBorderColor
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 28
            spacing: 24

            Text {
                text: qsTr("Panoramic Scan Configuration")
                font.pointSize: 20
                font.bold: true
                color: txtColor
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: panelBorderColor
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: 20
                columnSpacing: 24

                // =========================================================================
                // LABEL FOR THE FREQUENCY STRIP ROW
                // =========================================================================
                Label {
                    text: qsTr("Start Frequency:")
                    font.pointSize: 16
                    font.bold: true
                    color: textLabelColor
                    Layout.alignment: Qt.AlignVCenter
                }

                // =========================================================================
                // FULL FREQUENCY TUNING BUTTON STRIP (MATCHES VIEWMEASUREMENT ARCHITECTURE)
                // =========================================================================
                RowLayout {
                    id: rowTuningStrip
                    spacing: 8
                    Layout.fillWidth: true

                    Button {
                        id: btnDown3
                        text: "<<<"
                        Layout.preferredWidth: 100
                        font.bold: false
                        font.pointSize: 24
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_DOWN_LARGE)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                    Button {
                        id: btnDown2
                        text: "<<"
                        Layout.preferredWidth: 100
                        font.bold: false
                        font.pointSize: 24
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_DOWN_MEDIUM)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                    Button {
                        id: btnDown1
                        text: "<"
                        Layout.preferredWidth: 100
                        font.bold: false
                        font.pointSize: 24
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_DOWN_SMALL)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                    TextField {
                        id: txtFreq
                        text: formatFrequency()
                        placeholderText: qsTr("Frequency")

                        Layout.fillWidth: true
                        Layout.minimumWidth: 160
                        font {
                            pointSize: 24
                            family: "Monospace"
                            bold: true
                        }
                        transformOrigin: Item.TopLeft
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter

                        color: controlFontColor
                        background: Rectangle {
                            color: controlBackColor
                            border.color: controlBorderColor
                            radius: 4
                        }

                        inputMethodHints: Qt.ImhDigitsOnly

                        onActiveFocusChanged: {
                            if (backend) {
                                if (activeFocus) {
                                    // Clear out formatting text (" KHz") when user taps to edit
                                    // Break binding temporarily to allow raw digit entry
                                    text = Math.floor(backend.min_freq / 1000);
                                } else {
                                    // User tapped outside! Parse, validate, and commit to C++
                                    var typedKHz = parseInt(text.replace(/[^0-9]/g, ""));
                                    if (!Number.isNaN(typedKHz)) {
                                        backend.handleDirectFreqInput(typedKHz);
                                    }

                                    text = Qt.binding(formatFrequency);
                                }
                            }
                        }

                        // Relinquishes active focus cleanly when the user hits "Done/Enter"
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
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_UP_SMALL)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                    Button {
                        id: btnUp2
                        text: ">>"
                        Layout.preferredWidth: 100
                        font.bold: false
                        font.pointSize: 24
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_UP_MEDIUM)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                    Button {
                        id: btnUp3
                        text: ">>>"
                        Layout.preferredWidth: 100
                        font.bold: false
                        font.pointSize: 24
                        autoRepeat: true
                        onClicked: {
                            if (backend)
                                backend.handleTuneFrequency(PanoramicScanViewBackend.TUNE_UP_LARGE)
                        }
                        DesignEffect { effects: [ DesignDropShadow { } ] }
                    }

                } // </RowLayout>

                // =========================================================================
                // FREQUENCY INTERPRETATION MODE TOGGLE CONTROL
                // =========================================================================
                Label {
                    text: qsTr("Frequency Mode:")
                    font.pointSize: 16
                    font.bold: true
                    color: textLabelColor
                    Layout.alignment: Qt.AlignVCenter
                }

                RowLayout {
                    spacing: 16
                    Layout.fillWidth: true

                    Switch {
                        id: swFrequencyMode
                        checked: backend ? backend.is_min_freq_center : false

                        implicitWidth: 100
                        implicitHeight: 45

                        onToggled: {
                            if (backend) {
                                backend.is_min_freq_center = checked
                            }
                        }

                        indicator: Rectangle {
                            implicitWidth: swFrequencyMode.implicitWidth
                            implicitHeight: swFrequencyMode.implicitHeight
                            x: swFrequencyMode.leftPadding
                            y: parent.height / 2 - height / 2
                            radius: 4

                            // High-contrast pocket background colors matching your elevate look
                            color: swFrequencyMode.checked ? Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.15) : panelBackColor
                            border.color: swFrequencyMode.checked ? accentColor : panelBorderColor
                            border.width: 1.5

                            // The Sliding Handle Toggle Button Knob
                            Rectangle {
                                x: swFrequencyMode.checked ? parent.width - width - 6 : 6
                                y: parent.height / 2 - height / 2
                                width: 34
                                height: 34
                                radius: 3

                                // Color pops out bright when active, matches your theme gray when inactive
                                color: swFrequencyMode.checked ? accentColor : txtColor

                                // Add a smooth sliding transition effect when toggled
                                Behavior on x {
                                    NumberAnimation { duration: 180; easing.type: Easing.InOutQuad }
                                }
                            }
                        }

                        leftPadding: 0
                        rightPadding: 12

                        DesignEffect {
                            effects: [ DesignDropShadow { } ]
                        }
                    }

                    Text {
                        // Provides an obvious high-contrast text confirmation of the active synthesizer mode
                        text: swFrequencyMode.checked ? qsTr("CENTER Frequency Tracking Mode") : qsTr("START Frequency Tracking Mode")
                        font {
                            pointSize: 14
                            bold: true
                        }
                        color: swFrequencyMode.checked ? accentColor : txtColor
                    }
                }


                // =========================================================================
                // BANDWIDTH SELECTION SPAN SECTION
                // =========================================================================
                Label {
                    text: qsTr("Band Width Span:")
                    font.pointSize: 16
                    font.bold: true
                    color: textLabelColor
                    Layout.alignment: Qt.AlignVCenter
                }

                RowLayout {
                    spacing: 12
                    Layout.fillWidth: true

                    ComboBox {
                        id: cmbBandSpan
                        Layout.preferredWidth: 240
                        implicitHeight: 40

                        textRole: "text"
                        valueRole: "value"
                        font.pointSize: 16
                        font.bold: true

                        model: ListModel {
                            id: cbBandSpanModel
                            ListElement { text: "200 kHz";  value: PanoramicScanViewBackend._200K }
                            ListElement { text: "400 kHz";  value: PanoramicScanViewBackend._400K }
                            ListElement { text: "800 kHz";  value: PanoramicScanViewBackend._800K }
                            ListElement { text: "1.6 MHz";  value: PanoramicScanViewBackend._1600K }
                            ListElement { text: "2.0 MHz";  value: PanoramicScanViewBackend._2M }
                            ListElement { text: "4.0 MHz";  value: PanoramicScanViewBackend._4M }
                            ListElement { text: "8.0 MHz";  value: PanoramicScanViewBackend._8M }
                            ListElement { text: "16.0 MHz"; value: PanoramicScanViewBackend._16M }
                            ListElement { text: "20.0 MHz"; value: PanoramicScanViewBackend._20M }
                            ListElement { text: "40.0 MHz"; value: PanoramicScanViewBackend._40M }
                            ListElement { text: "80.0 MHz"; value: PanoramicScanViewBackend._80M }
                        }

                        contentItem: Text {
                            id: cmbBandSpanText
                            rightPadding: cmbBandSpan.indicator.width + cmbBandSpan.spacing

                            text: cmbBandSpan.displayText
                            font: cmbBandSpan.font
                            color: cmbBandSpan.pressed ? AapiTheme.style.controlFontColor : AapiTheme.style.controlPressedFontColor
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }

                        delegate: ItemDelegate {
                            id: cmbBandSpanDelegate

                            required property var model
                            required property int index

                            height: 40
                            width: cmbBandSpan.width
                            contentItem: Text {
                                text: cmbBandSpanDelegate.model[cmbBandSpan.textRole]
                                font: cmbBandSpan.font
                                color: AapiTheme.style.controlFontColor
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                            highlighted: cmbBandSpan.highlightedIndex === index
                        }

                        popup: Popup {
                            id: cmbBandSpanPopup
                            y: cmbBandSpan.height
                            width: cmbBandSpan.width

                            height: Math.min(cmbBandSpan.model.count, 8) * 40 + 2
                            padding: 1

                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: cmbBandSpan.popup.visible ? cmbBandSpan.delegateModel : null
                                currentIndex: cmbBandSpan.highlightedIndex

                                ScrollIndicator.vertical: ScrollIndicator {
                                    id: cmbBandSpanVScroll
                                    active: true
                                    width: AapiTheme.style.scrollBarWidth ?? 4
                                }
                            }
                        }

                        DesignEffect {
                            effects: [ DesignInnerShadow { }, DesignDropShadow { } ]
                        }

                        Connections {
                            target: backend
                            function onBandSpanChanged() {
                                var cur_sel = 0
                                for (var i = 0; i < cmbBandSpan.model.count; i++) {
                                    if (backend.band_span == cmbBandSpan.model.get(i).value) {
                                        cur_sel = i;
                                        break;
                                    }
                                }
                                cmbBandSpan.currentIndex = cur_sel;
                            }
                        }

                        onActivated: {
                            var selectedValue = model.get(currentIndex).value
                            backend.band_span = selectedValue
                        }

                        Component.onCompleted: {
                            var cur_sel = 0
                            for (var i = 0; i < model.count; i++) {
                                if (backend.band_span == model.get(i).value) {
                                    cur_sel = i;
                                    break;
                                }
                            }
                            cmbBandSpan.currentIndex = cur_sel;
                        }
                    }
                }
            } // </GridLayout>

            Item { Layout.fillHeight: true } // Form separation spacing spacer

            // =========================================================================
            // DYNAMIC DETAILED SCAN COVERAGE LEGEND LABEL
            // =========================================================================
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                color: Qt.rgba(0, 0, 0, 0.2)
                radius: 4
                border.color: panelBorderColor

                Text {
                    id: lblScanRangeDetails
                    anchors.centerIn: parent
                    font {
                        pointSize: 13
                        family: "Monospace"
                        bold: true
                    }
                    color: txtColor

                    text: {
                        if (!backend) return "";

                        // Convert your current raw frequency from Hz down to kHz
                        var freqKHz = Math.floor(backend.min_freq / 1000);
                        var spanKHz = backend.band_span;

                        if (backend.is_min_freq_center) {
                            // Mode A: Center Frequency Symmetrical Distribution
                            var startBound = freqKHz - Math.floor(spanKHz / 2);
                            var endBound = freqKHz + Math.floor(spanKHz / 2);

                            return qsTr("Sweep Boundaries (Center Mode): %1 kHz  =>  %2 kHz")
                                   .arg(startBound)
                                   .arg(endBound);
                        } else {
                            // Mode B: Standard Base Start Frequency Distribution
                            return qsTr("Sweep Boundaries (Start Mode): %1 kHz  =>  %2 kHz")
                                   .arg(freqKHz)
                                   .arg(freqKHz + spanKHz);
                        }
                    }
                }
            }
        } // </ColumnLayout>
    } // </Rectangle>

    // Control locking mapping routine for hardware sweeps
    function enableControls(enable) {
        btnDown3.enabled = enable
        btnDown2.enabled = enable
        btnDown1.enabled = enable
        txtFreq.enabled = enable
        btnUp1.enabled = enable
        btnUp2.enabled = enable
        btnUp3.enabled = enable
        cmbBandSpan.enabled = enable
    }

    function formatFrequency() {
        return backend ? (Math.floor(backend.min_freq / 1000) + " KHz") : "14000 KHz";
    }
} // </Item>