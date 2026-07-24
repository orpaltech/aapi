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
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi


SwipePage {
    id: viewOslCal
    title: qsTr("OSL Calibration")
    backend: aapi.view_osl_calibration

    property alias layoutOslCal: layoutOslCal
    readonly property int optionHeight: 42

    readonly property color textLabelColor: AapiTheme.style.textLabelColor
    readonly property color resistanceLabelColor: AapiTheme.style.highContrastRedColor
    // Color logic: Red if dead signal (~0), Orange if saturated (>90), Teal if safe
    readonly property color telemetryDeadColor: AapiTheme.style.telemetryDeadColor
    readonly property color telemetryHighColor: AapiTheme.style.telemetryAlertColor
    readonly property color telemetryLabelColor: AapiTheme.style.accentMutedColor

    ColumnLayout {
        id: layoutOslCal
        width: 1280
        height: 656
        spacing: 0

        RowLayout {
            id: row1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.topMargin: 30
            spacing: 20

            Label {
                id: labelOSLFile
                width: 120
                text: qsTr("Select Calibration File:")
                font.pointSize: 20
                horizontalAlignment: Text.AlignRight
            }

            ComboBox {
                id: cbOslFile
                implicitHeight: optionHeight
                textRole: "text"
                valueRole: "value"
                font.pointSize: 22
                model: ListModel {
                    id: cbOslFileModel
                }

                contentItem: Text {
                    id: cbOslFileText
                    /*leftPadding: 0*/
                    rightPadding: cbOslFile.indicator.width + cbOslFile.spacing

                    text: cbOslFile.displayText
                    font: cbOslFile.font
                    color: cbOslFile.pressed ? AapiTheme.style.controlFontColor : AapiTheme.style.controlPressedFontColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }

                delegate: ItemDelegate {
                    id: cbOslFileDelegate

                    required property var model
                    required property int index

                    height: optionHeight
                    width: cbOslFile.width
                    contentItem: Text {
                        text: cbOslFileDelegate.model[cbOslFile.textRole]
                        font: cbOslFile.font
                        color: AapiTheme.style.controlFontColor
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                    highlighted: cbOslFile.highlightedIndex === index
                }
                popup.height: Math.min(backend.num_osl_files, 14) * optionHeight + 2
                popup.padding: 1

                onCurrentIndexChanged: {
                    if (cbOslFile.currentIndex < 0)
                        return;

                    backend.osl_file = cbOslFile.model.get(cbOslFile.currentIndex).value;
                }

                DesignEffect {
                    effects: [ DesignDropShadow { } ]
                }
            }

            Label {
                id: labelOslFileExist
                width: 145
                text: qsTr("File exists")
                font.pointSize: 18
            }
        }

        RowLayout {
            id: row2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            transformOrigin: Item.TopLeft
            spacing: 12

            Frame {
                id: frame1
                contentWidth: 980
                contentHeight: 420

                // Split the frame horizontally: Left for Button controls, Right for telemetry readouts
                RowLayout {
                    anchors.fill: parent
                    spacing: 30

                    // Left Block: The 3 calibration options pulled closer together
                    ColumnLayout {
                        id: colControls
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 30

                        // --- OPEN ROW --->>
                        RowLayout {
                            id: row23
                            spacing: 16

                            TwoStateButtonCtrl {
                                id: btnOpenScan
                                implicitWidth: 160
                                offText: qsTr("Start")
                                onText: qsTr("Cancel")
                                font.pointSize: 20

                                function onStart() {
                                    console.log("OSL Cal: onStart Initiated")
                                    scanDialog.show(OSLCalibrationViewBackend.SCAN_OPEN);
                                    return false;
                                }
                                function onStop() {
                                    console.log("OSL Cal: onStop Interrupted")
                                    backend.handleCancelScan();
                                    statOpen.state = "disabled";
                                }

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }

                            Label {
                                id: labelOpen
                                color: textLabelColor
                                text: qsTr("[O]PEN")
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            Label {
                                id: labelOpenR
                                color: resistanceLabelColor
                                text: qsTr("10 Ohm")
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                horizontalAlignment: Text.AlignRight
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            StatusIndicatorCtrl {
                                id: statOpen
                                width: 36;
                                height: 36

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }
                        }
                        // <<--- OPEN ROW ---

                        // --- SHORT ROW --->>
                        RowLayout {
                            id: row21
                            spacing: 16

                            TwoStateButtonCtrl {
                                id: btnShortScan
                                implicitWidth: 160
                                offText: qsTr("Start")
                                onText: qsTr("Cancel")
                                font.pointSize: 20

                                function onStart() {
                                    console.log("OSL Cal: onStart Initiated")
                                    scanDialog.show(OSLCalibrationViewBackend.SCAN_SHORT);
                                    return false;
                                }
                                function onStop() {
                                    console.log("OSL Cal: onStop Interrupted")
                                    backend.handleCancelScan();
                                    statShort.state = "disabled";
                                }

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }

                            Label {
                                id: labelShort
                                color: textLabelColor
                                text: qsTr("[S]HORT")
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            Label {
                                id: labelShortR
                                color: resistanceLabelColor
                                text: qsTr("10 Ohm")
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                horizontalAlignment: Text.AlignRight
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            StatusIndicatorCtrl {
                                id: statShort
                                width: 36;
                                height: 36

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }
                        }
                        // <<--- SHORT ROW ---

                        // --- LOAD ROW --->>
                        RowLayout {
                            id: row22
                            spacing: 16

                            TwoStateButtonCtrl {
                                id: btnLoadScan
                                implicitWidth: 160
                                offText: qsTr("Start")
                                onText: qsTr("Cancel")
                                font.pointSize: 20

                                function onStart() {
                                    console.log("OSL Cal: onStart Initiated")
                                    scanDialog.show(OSLCalibrationViewBackend.SCAN_LOAD);
                                    return false;
                                }
                                function onStop() {
                                    console.log("OSL Cal: onStop Interrupted")
                                    backend.handleCancelScan();
                                    statLoad.state = "disabled";
                                }

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }

                            Label {
                                id: labelLoad
                                text: qsTr("[L]OAD")
                                color: textLabelColor
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            Label {
                                id: labelLoadR
                                color: resistanceLabelColor
                                text: qsTr("10 Ohm")
                                font {
                                    pointSize: 22;
                                    bold: true;
                                }
                                horizontalAlignment: Text.AlignRight
                                Layout.fillWidth: false
                                Layout.preferredWidth: 140
                            }

                            StatusIndicatorCtrl {
                                id: statLoad
                                width: 36;
                                height: 36

                                DesignEffect {
                                    effects: [ DesignDropShadow { } ]
                                }
                            }
                        }
                        // <<--- LOAD ROW ---
                    }

                    // Vertical Separator Bar
                    Rectangle {
                        Layout.fillHeight: true
                        width: 2
                        color: "#dddddd"
                        Layout.topMargin: 20
                        Layout.bottomMargin: 20
                    }

                    // Right Block: Real-time Telemetry parameters
                    ColumnLayout {
                        id: colTelemetry
                        Layout.preferredWidth: 360
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 20

                        Label {
                            text: qsTr("Live Scan Parameters")
                            font {
                                pointSize: 22;
                            }
                            color: textLabelColor
                            Layout.bottomMargin: 10
                        }

                        // Frequency Readout Block
                        RowLayout {
                            spacing: 10
                            Label {
                                text: qsTr("Frequency:")
                                font {
                                    pointSize: 18;
                                    bold: true
                                }
                                Layout.preferredWidth: 130
                            }
                            Label {
                                id: lblTelemetryFreq
                                text: qsTr("0.000 MHz")
                                font {
                                    pointSize: 18;
                                    bold: true
                                    family: "Monospace"
                                }
                                color: telemetryLabelColor
                            }
                        }

                        // RX Real Part Readout Block
                        RowLayout {
                            spacing: 10
                            Label {
                                text: qsTr("RX Real:")
                                font {
                                    pointSize: 18;
                                    bold: true
                                }
                                Layout.preferredWidth: 130
                            }
                            Label {
                                id: lblTelemetryRxRe
                                property real val: 0.0
                                text: val.toFixed(5)
                                font {
                                    pointSize: 18;
                                    bold: true
                                    family: "Monospace"
                                }

                                // Color logic: Red if dead signal (~0), Orange if saturated (>90), Teal if safe
                                color: Math.abs(val) < 0.005 ? telemetryDeadColor :
                                        ((Math.abs(val) > 90.0 ? telemetryHighColor : telemetryLabelColor))
                            }
                        }

                        // RX Imaginary Part Readout Block
                        RowLayout {
                            spacing: 10
                            Label {
                                text: qsTr("RX Imag:")
                                font {
                                    pointSize: 18;
                                    bold: true
                                }
                                Layout.preferredWidth: 130
                            }
                            Label {
                                id: lblTelemetryRxIm
                                property real val: 0.0
                                text: val.toFixed(5)
                                font {
                                    pointSize: 18;
                                    bold: true
                                    family: "Monospace"
                                }

                                // Color logic: Red if dead signal (~0), Orange if saturated (>90), Teal if safe
                                color: Math.abs(val) < 0.005 ? telemetryDeadColor :
                                        ((Math.abs(val) > 90.0 ? telemetryHighColor : telemetryLabelColor))
                            }
                        }

                    }
                }
            }
        }

        RowLayout {
            id: row5
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.bottomMargin: 30

            ProgressCtrl {
                id: scanProgress
                height: 36
                Layout.fillWidth: true
                Layout.rightMargin: 140
                Layout.leftMargin: 140
                // Binds dynamically to your theme architecture variables
                color: AapiTheme.style.progressGradientColor1
                secondColor: AapiTheme.style.progressGradientColor2
                value: 0
            }
        }
    }

    AapiMessageBox {
        id: scanDialog
        caption: qsTr("")
        iconType: "exclamation"
        detailedText: qsTr("")
        messageTextSize: 18
        standardButtons: Dialog.Ok | Dialog.Cancel

        property int scanType

        onAccepted: {
            if (!backend.handleStartScan(scanType)) {
                return;
            }
            switch (scanType) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                statShort.state = "busy";
                btnShortScan.setOn();
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                statLoad.state = "busy";
                btnLoadScan.setOn()
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                statOpen.state = "busy";
                btnOpenScan.setOn();
                break;
            }

            enableControls(scanType);
        }

        function show(type) {
            scanType = type;
            switch (scanType) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                caption = qsTr("Scan Short");
                messageText = labelMsgText(backend.r_short);
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                caption = qsTr("Scan Load");
                messageText = labelMsgText(backend.r_load);
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                caption = qsTr("Scan Open");
                messageText = labelMsgText(backend.r_open);
                break;
            }
            open();
        }

        function labelMsgText(resistance) {
            return qsTr("Please, connect calibration load " + resistance + " Ohm and tap OK");
        }
    }

    AapiMessageBox {
        id: scanError
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

    onLoaded: {
        if (!backend) return;

        // Clear old values safely
        cbOslFile.model.clear();

        // Populate combobox options
        var curSel = 0;

        // Safely unpack all options
        for (var i = 0; i < backend.num_osl_files; i++) {
            var el = {
                text: backend.file_opt_labels[i],
                value: backend.file_opt_values[i]
            };
            cbOslFile.model.append(el);

            // If the value matches the active backend file selection, lock it down
            if (backend.osl_file === backend.file_opt_values[i]) {
                curSel = i;
            }
        }
        // Update selection marker
        cbOslFile.currentIndex = curSel;

        // Static resistance texts
        labelShortR.text = backend.r_short + " Ohm";
        labelLoadR.text  = backend.r_load  + " Ohm";
        labelOpenR.text  = backend.r_open  + " Ohm";

        // Force a manual evaluation cycle immediately upon boot completion
        updateOslFileExists();
    }

    Connections {
        target: backend
        ignoreUnknownSignals: true

        function onScanProgress(scan_type, step, total, freq, rx_re, rx_im) {
            console.log("OSL calibration scan progress");

            scanProgress.minimum = 0;
            scanProgress.maximum = total;
            scanProgress.value = step;

            // Format frequency from C++ raw Hz directly up to readable MHz
            var currentMHz = freq / 1000000.0
            lblTelemetryFreq.text = currentMHz.toFixed(3) + " MHz"
            // Push raw numeric variables into telemetry components to update labels and trigger colors
            lblTelemetryRxRe.val = rx_re;
            lblTelemetryRxIm.val = rx_im;

            if (step === total) { // scan finished
                enableControls();

                switch (scan_type) {
                case OSLCalibrationViewBackend.SCAN_OPEN:
                    statOpen.state = "success";
                    btnOpenScan.setOff();
                    break;
                case OSLCalibrationViewBackend.SCAN_SHORT:
                    statShort.state = "success";
                    btnShortScan.setOff();
                    break;
                case OSLCalibrationViewBackend.SCAN_LOAD:
                    statLoad.state = "success";
                    btnLoadScan.setOff();
                    break;
                }
            }
        }

        function onScanNoSignal(scan_type) {
            scanError.show(qsTr("Low signal. Please, check hardware."));

            enableControls();

            switch (scan_type) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                statShort.state = "disabled";
                btnShortScan.setOff();
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                statLoad.state = "disabled";
                btnLoadScan.setOff();
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                statOpen.state = "disabled";
                btnOpenScan.setOff();
                break;
            }
        }

        function onScanError(message) {
            scanError.show(message);
        }

        function onOslFileChanged() {
            updateOslFileExists();

            // invalidate previous OSL calibration attempts made
            statShort.state = "disabled";
            statLoad.state = "disabled";
            statOpen.state = "disabled";
        }

        function onOslFileExistsChanged() {
            updateOslFileExists();
        }
    }


    function updateOslFileExists() {
        if (!backend || typeof labelOslFileExist === "undefined") return;

        // Guard Check: If "None" (-1) is selected
        if (backend.osl_file === -1) {
            labelOslFileExist.text = qsTr("Calibration Inactive");
            return;
        }

        // Dynamic file system existence checks
        if (backend.osl_file_exists)
            labelOslFileExist.text = qsTr("File exists");
        else
            labelOslFileExist.text = qsTr("File not found");
    }

    function enableControls(currentScanType) {

        if (typeof currentScanType === "undefined"
                || currentScanType === null) {

            btnOpenScan.enabled = true;
            btnShortScan.enabled = true;
            btnLoadScan.enabled = true;
            cbOslFile.enabled = true;

        } else {
            cbOslFile.enabled = false;

            switch (currentScanType) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                btnOpenScan.enabled = false;
                btnShortScan.enabled = true;
                btnLoadScan.enabled = false;
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                btnOpenScan.enabled = false;
                btnShortScan.enabled = false;
                btnLoadScan.enabled = true;
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                btnOpenScan.enabled = true;
                btnShortScan.enabled = false;
                btnLoadScan.enabled = false;
                break;
            }
        }
    }
}
