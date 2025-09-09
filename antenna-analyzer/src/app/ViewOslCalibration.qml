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
    property alias layoutOslCal: layoutOslCal
    readonly property int optionHeight: 42

    id: viewOslCal
    title: "OSL Calibration"
    backend: aapi.view_osl_calibration

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
                width: 105
                text: qsTr("Calibration File:")
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
                    leftPadding: 0
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

                    statShort.state = "disabled";
                    statLoad.state = "disabled";
                    statOpen.state = "disabled";

                    updateOslFileExists();
                }

                DesignEffect {
                    effects: [
                        DesignDropShadow {
                        }
                    ]
                }
            }
            Label {
                id: labelOslFileExist
                width: 145
                text: qsTr("File exists")
                font.pointSize: 16
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

                ColumnLayout {
                    id: col21
                    anchors.fill: parent

                    RowLayout {
                        id: row21
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        spacing: 24

                        Label {
                            id: labelShort
                            text: qsTr("Scan Short:")
                            font.pointSize: 24
                            horizontalAlignment: Text.AlignRight
                            textFormat: Text.PlainText
                            font.bold: true
                        }
                        Label {
                            id: labelShortR
                            color: "#af0404"
                            text: qsTr("10 Ohm")
                            font.bold: true
                            font.pointSize: 24
                        }
                        StatusIndicatorCtrl {
                            id: statShort
                            width: 40
                            height: 40

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
                            }
                        }
                        TwoStateButtonCtrl {
                            id: btnShortScan
                            implicitWidth: 160
                            font.italic: false
                            font.bold: false
                            onText: qsTr("Scan")
                            on: false
                            offText: qsTr("Scan")
                            font.pointSize: 26

                            function onStart() {
                                scanDialog.show(OSLCalibrationViewBackend.SCAN_SHORT);
                                return false;
                            }
                            function onStop() {
                                backend.cancel_scan();
                                statShort.state = "disabled";
                            }

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
                            }
                        }
                    }

                    RowLayout {
                        id: row22
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        spacing: 24

                        Label {
                            id: labelLoad
                            text: qsTr("Scan Load:")
                            font.pointSize: 24
                            horizontalAlignment: Text.AlignRight
                            textFormat: Text.PlainText
                            font.bold: true
                        }
                        Label {
                            id: labelLoadR
                            color: "#af0404"
                            text: qsTr("10 Ohm")
                            font.pointSize: 24
                            font.bold: true
                        }
                        StatusIndicatorCtrl {
                            id: statLoad
                            width: 40
                            height: 40

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
                            }
                        }
                        TwoStateButtonCtrl {
                            id: btnLoadScan
                            implicitWidth: 160
                            offText: qsTr("Scan")
                            onText: qsTr("Cancel")
                            font.pointSize: 26

                            function onStart() {
                                scanDialog.show(OSLCalibrationViewBackend.SCAN_LOAD);
                                return false;
                            }
                            function onStop() {
                                backend.cancel_scan();
                                statLoad.state = "disabled";
                            }

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
                            }
                        }
                    }

                    RowLayout {
                        id: row23
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        spacing: 24

                        Label {
                            id: labelOpen
                            text: qsTr("Scan Open:")
                            font.pointSize: 24
                            horizontalAlignment: Text.AlignRight
                            textFormat: Text.PlainText
                            font.bold: true
                            transformOrigin: Item.TopLeft
                        }
                        Label {
                            id: labelOpenR
                            color: "#af0404"
                            text: qsTr("10 Ohm")
                            font.pointSize: 24
                            font.bold: true
                        }
                        StatusIndicatorCtrl {
                            id: statOpen
                            width: 40
                            height: 40

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
                            }
                        }
                        TwoStateButtonCtrl {
                            id: btnOpenScan
                            implicitWidth: 160
                            offText: qsTr("Scan")
                            onText: qsTr("Cancel")
                            font.pointSize: 26

                            function onStart() {
                                scanDialog.show(OSLCalibrationViewBackend.SCAN_OPEN);
                                return false;
                            }
                            function onStop() {
                                backend.cancel_scan();
                                statOpen.state = "disabled";
                            }

                            DesignEffect {
                                effects: [
                                    DesignDropShadow {
                                    }
                                ]
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
                height: 38
                color: "#006866"
                secondColor: "#25fffc"
                value: 100
                Layout.rightMargin: 141
                Layout.leftMargin: 141
                Layout.fillWidth: true
            }
        }
    }

    AapiMessageBox {
        id: scanDialog
        caption: qsTr("Scan")
        iconType: "exclamation"
        detailedText: ""
        standardButtons: Dialog.Ok | Dialog.Cancel

        property int scanType

        onAccepted: {
            if (backend.start_scan(scanType)) {
                // TODO: display error
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

            enableControls(scanType, false);
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

/*
    Dialog {
        property int scanType

        id: scanDialog2
        title: qsTr("OSL Calibration")
        modal: true
        font.pointSize: 10
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        scale: 1.2

        Label {
            id: labelMsg
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (backend.start_scan(scanType)) {
                // TODO: display error
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

            enableControls(scanType, false);
        }


        function show(type) {
            scanType = type;
            switch (scanType) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                title = qsTr("Scan Short");
                labelMsg.text = labelMsgText(backend.r_short);
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                title = qsTr("Scan Load");
                labelMsg.text = labelMsgText(backend.r_load);
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                title = qsTr("Scan Open");
                labelMsg.text = labelMsgText(backend.r_open);
                break;
            }
            open();
        }
    }
*/

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

    onLoaded: {
        // populate combobox options
        cbOslFile.model.clear();
        var curSel = 0;
        for (var i = 0; i < backend.num_osl_files; i++) {
            var el = {
                text: backend.file_opt_labels[i],
                value: backend.file_opt_values[i]
            };
            cbOslFile.model.append(el);
            if (backend.osl_file === backend.file_opt_values[i]) {
                curSel = i;
            }
        }
        cbOslFile.currentIndex = curSel;
        cbOslFile.update();

        labelShortR.text = backend.r_short + " Ohm";
        labelLoadR.text = backend.r_load + " Ohm";
        labelOpenR.text = backend.r_open + " Ohm";
    }

    Connections {
        target: backend
        function onScanProgress() {
            updateScanProgress(type, step, total);
        }
        function onScanNoSignal() {
            updateScanNoSignal(type);
        }
        function onOslFileExistsChanged() {
            updateOslFileExists();
        }
    }

    function updateScanNoSignal(type) {
        scanError.show(qsTr("Low signal. Please, check hardware."));

        enableControls(type, true);

        switch (type) {
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

    function updateScanProgress(type, step, total) {
        console.log("OSL calibration scan progress");
        scanProgress.minimum = 0;
        scanProgress.maximum = total;
        scanProgress.value = step;

        if (step === total) { /* scan finished */
            enableControls(type, true);

            switch (type) {
            case OSLCalibrationViewBackend.SCAN_SHORT:
                statShort.state = "success";
                btnShortScan.setOff();
                break;
            case OSLCalibrationViewBackend.SCAN_LOAD:
                statLoad.state = "success";
                btnLoadScan.setOff();
                break;
            case OSLCalibrationViewBackend.SCAN_OPEN:
                statOpen.state = "success";
                btnOpenScan.setOff();
                break;
            }
        }
    }

    function updateOslFileExists() {
        if (backend.osl_file_exists)
            labelOslFileExist.text = qsTr("File exists")
        else
            labelOslFileExist.text = qsTr("File not found")
    }

    function enableControls(scanType, enable) {
        cbOslFile.enabled = enable;

        switch (scanType) {
        case OSLCalibrationViewBackend.SCAN_SHORT:
            btnLoadScan.enabled = enable;
            btnOpenScan.enabled = enable;
            break;
        case OSLCalibrationViewBackend.SCAN_LOAD:
            btnShortScan.enabled = enable;
            btnOpenScan.enabled = enable;
            break;
        case OSLCalibrationViewBackend.SCAN_OPEN:
            btnShortScan.enabled = enable;
            btnLoadScan.enabled = enable;
            break;
        }
    }
}
