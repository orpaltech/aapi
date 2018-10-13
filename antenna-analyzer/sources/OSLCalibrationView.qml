/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

SwipePage {
    backend: analyzer.oslcal

    ColumnLayout {
        id: layoutOSLCal
        anchors {
            fill: parent
            leftMargin: 30
            rightMargin: 40
            topMargin: 5
            bottomMargin: 5
        }
        spacing: 12

        Row {
            id: row1
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: labelOSLFile
                width: 105
                text: "OSL File:"
                font.pointSize: 14
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
            }

            ComboBox {
                id: cbOSLFile
                width: 100
                font.pointSize: 12
                textRole: "text"
                model: ListModel {}
            }

            Label {
                id: labelOSLFileExist
                width: 145
                text: "File exists"
                font.pointSize: 12
                anchors.verticalCenter: parent.verticalCenter

            }
        }

        Row {
            id: row2
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: labelShort
                width: 105
                text: "Scan Short:"
                font.pointSize: 13
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
            }

            ComboBox {
                id: cbOSLShort
                width: 100
                font.pointSize: 12
                textRole: "text"
                model: ListModel {
                    id: cbOSLShortModel
                }
            }

            StatusIndicatorCtrl {
                id: siOSLShort
                width: 26
                height: 26
                anchors.verticalCenter: parent.verticalCenter
            }

            TwoStateButtonCtrl {
                id: buttonShort
                width: 105
                offText: "Scan"
                onText: "Cancel"
                font.pointSize: 12

                function onStart() {
                    scanDialog.show(OSLcalBackend.SCAN_SHORT);
                    return false;
                }
                function onStop() {
                    backend.cancel_scan();
                    siOSLShort.state = "disabled";
                }
            }
        }

        Row {
            id: row3
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: labelLoad
                width: 105
                text: "Scan Load:"
                font.pointSize: 13
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
            }

            ComboBox {
                id: cbOSLLoad
                width: 100
                font.pointSize: 12
                textRole: "text"
                model: ListModel {
                    id: cbOSLLoadModel
                }
            }

            StatusIndicatorCtrl {
                id: siOSLLoad
                width: 26
                height: 26
                anchors.verticalCenter: parent.verticalCenter
            }

            TwoStateButtonCtrl {
                id: buttonLoad
                width: 105
                offText: "Scan"
                onText: "Cancel"
                font.pointSize: 12

                function onStart() {
                    scanDialog.show(OSLcalBackend.SCAN_LOAD);
                    return false;
                }
                function onStop() {
                    backend.cancel_scan();
                    siOSLLoad.state = "disabled";
                }
            }
        }

        Row {
            id: row4
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: labelOpen
                width: 105
                text: "Scan Open:"
                font.pointSize: 13
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
            }

            ComboBox {
                id: cbOSLOpen
                width: 100
                font.pointSize: 12
                textRole: "text"
                model: ListModel {
                    id: cbOSLOpenModel
                }
            }

            StatusIndicatorCtrl {
                id: siOSLOpen
                width: 26
                height: 26
                anchors.verticalCenter: parent.verticalCenter
            }

            TwoStateButtonCtrl {
                id: buttonOpen
                width: 105
                offText: "Scan"
                onText: "Cancel"
                font.pointSize: 12

                function onStart() {
                    scanDialog.show(OSLcalBackend.SCAN_OPEN);
                    return false;
                }
                function onStop() {
                    backend.cancel_scan();
                    siOSLOpen.state = "disabled";
                }
            }
        }

        ProgressCtrl {
            id: progressOSLCal
            height: 28
            color: "#505268"
            secondColor: "#8286a9"
            value: 0
            Layout.fillWidth: true
        }
    }

    Dialog {
        property int scanType

        id: scanDialog
        title: "OSL Scan"
        modal: true
        font.pointSize: 10
        x: (Screen.width - scanDialog.width)/2
        y: (Screen.height - scanDialog.height)/2
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            id: labelMsg
            wrapMode: Text.WordWrap
        }

        onAccepted: startScan()
        onRejected: {

        }

        function startScan() {
            if (backend.start_scan(scanType)) {
                // TODO: display error
                return;
            }
            switch (scanType) {
            case OSLcalBackend.SCAN_SHORT:
                siOSLShort.state = "busy";
                buttonShort.setOn();
                break;
            case OSLcalBackend.SCAN_LOAD:
                siOSLLoad.state = "busy";
                buttonLoad.setOn()
                break;
            case OSLcalBackend.SCAN_OPEN:
                siOSLOpen.state = "busy";
                buttonOpen.setOn();
                break;
            }

            enableControls(scanType, false);
        }

        function labelMsgText(r) {
            return "Please, connect calibration load " + r + " Ohm and tap OK";
        }

        function show(type) {
            scanType = type;
            switch (scanType) {
            case OSLcalBackend.SCAN_SHORT:
                title = "Scan Short";
                labelMsg.text = labelMsgText(backend.r_short);
                break;
            case OSLcalBackend.SCAN_LOAD:
                title = "Scan Load";
                labelMsg.text = labelMsgText(backend.r_load);
                break;
            case OSLcalBackend.SCAN_OPEN:
                title = "Scan Open";
                labelMsg.text = labelMsgText(backend.r_open);
                break;
            }
            open();
        }
    }

    Dialog {
        id: scanError
        title: "Scan Error"
        modal: true
        font.pointSize: 10
        x: (Screen.width - scanError.width)/2
        y: (Screen.height - scanError.height)/2
        standardButtons: Dialog.Ok

        Label {
            id: labelScanError
        }

        function show(message) {
            labelScanError.text = message
            open()
        }
    }

    onLoaded: {
        setupOslFile()
    }


    Connections {
        target: backend

        onScanProgress: {
            scanProgress(type, step, total)
        }
        onScanNoSignal: {
            scanNoSignal(type)
        }
        onOslFileExistsChanged: {
            labelOSLFileExist.text = "File exists"
        }
    }

    Connections {
        target: cbOSLFile
        onCurrentIndexChanged: {
            if (cbOSLFile.currentIndex >= 0) {
                backend.osl_file = cbOSLFile.model.get(cbOSLFile.currentIndex).value
                labelOSLFileExist.text = backend.file_exists ? "File exists" : ""
                siOSLShort.state = "disabled"
                siOSLLoad.state = "disabled"
                siOSLOpen.state = "disabled"
            }
        }
    }

    function scanNoSignal(type) {
        scanError.show("Low signal. Please, check hardware.");

        enableControls(type, true);

        switch (type) {
        case OSLcalBackend.SCAN_SHORT:
            siOSLShort.state = "disabled";
            buttonShort.setOff();
            break;
        case OSLcalBackend.SCAN_LOAD:
            siOSLLoad.state = "disabled";
            buttonLoad.setOff();
            break;
        case OSLcalBackend.SCAN_OPEN:
            siOSLOpen.state = "disabled";
            buttonOpen.setOff();
            break;
        }
    }

    function scanProgress(type, step, total) {
        console.log("OSL calibration scan progress");
        progressOSLCal.minimum = 0;
        progressOSLCal.maximum = total;
        progressOSLCal.value = step;

        if (step === total) { /* scan finished */
            enableControls(type, true);

            switch (type) {
            case OSLcalBackend.SCAN_SHORT:
                siOSLShort.state = "success";
                buttonShort.setOff();
                break;
            case OSLcalBackend.SCAN_LOAD:
                siOSLLoad.state = "success";
                buttonLoad.setOff();
                break;
            case OSLcalBackend.SCAN_OPEN:
                siOSLOpen.state = "success";
                buttonOpen.setOff();
                break;
            }
        }
    }

    function enableControls(scanType, enable) {
        cbOSLFile.enabled = enable;

        switch (scanType) {
        case OSLcalBackend.SCAN_SHORT:
            buttonLoad.enabled = enable;
            buttonOpen.enabled = enable;
            break;
        case OSLcalBackend.SCAN_LOAD:
            buttonShort.enabled = enable;
            buttonOpen.enabled = enable;
            break;
        case OSLcalBackend.SCAN_OPEN:
            buttonShort.enabled = enable;
            buttonLoad.enabled = enable;
            break;
        }
    }

    function setupOslFile() {
        var opt_labels = backend.file_opt_labels;
        var opt_values = backend.file_opt_values;
        cbOSLFile.model.clear();
        var cur_sel = 0;
        for (var i = 0; i < backend.file_num_options; i++) {
            var el = {
                text: opt_labels[i],
                value: opt_values[i]
            };
            cbOSLFile.model.append(el);
            if (backend.osl_file === opt_values[i]) {
                cur_sel = i;
            }
        }
        cbOSLFile.currentIndex = cur_sel;
        cbOSLFile.update();
    }
}
