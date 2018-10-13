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
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

SwipePage {
    backend: analyzer.hwcal

    ColumnLayout {
        id: layoutHWCal
        spacing: 5
        anchors{
            leftMargin: 40
            topMargin: 5
            rightMargin: 40
            bottomMargin: 10
            fill: parent
        }

        Label {
            id: labelHWCal
            text: "Set PCB jumper to HW Calibration position and press Start button"
            wrapMode: Text.WordWrap
            font.pointSize: 14
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        ProgressCtrl {
            id: progressHWCal
            height: 36
            Layout.fillWidth: true
            color: "#505268"
            secondColor: "#8286a9"
            value: 0
        }

        TwoStateButtonCtrl {
            id: buttonHWCal
            offText: "Start Calibration"
            onText: "Stop Calibration"
            Layout.maximumWidth: 200
            Layout.preferredWidth: 200
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: false

            function onStart() {
                console.log("HW Cal: onStart")
                return backend.start_hwcal() === 0
            }

            function onStop() {
                console.log("HW Cal: onStop")
                backend.cancel_hwcal()
            }
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

    Connections {
        target: backend
        onScanProgress: {
            console.log("HW calibration scan progress")
            progressHWCal.minimum = 0
            progressHWCal.maximum = total
            progressHWCal.value = step
            if (step == total) {
                buttonHWCal.setOff()
            }
        }

        onScanNoSignal: {
            scanError.show("Low signal. Please, check hardware.")
            buttonHWCal.setOff()
        }

    }
}
