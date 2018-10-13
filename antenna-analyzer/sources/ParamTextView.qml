/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Styles 2.1
import com.orpaltech.aapi 1.0


Item {
    property ConfigViewBackend backend: analyzer.config

    id: itemControl
    anchors.fill: parent

    Frame {
        id: frameControl
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: inputPanel.top

        ColumnLayout {
            id: layoutControl
            anchors.fill: parent

            Label {
                id: labelTitle
                text: "here will be the parameter description"
                font.pointSize: 14
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            TextField {
                id: textValue
                focus: true
                text: "here will be the parameter value"
                font.pointSize: 16
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                Layout.fillWidth: true
            }
        }
    }

    InputPanel {
        id: inputPanel
        z: 99
        opacity: 0
        y: itemControl.height
        anchors.left: parent.left
        anchors.right: parent.right

        states: State {
            name: "visible"
            when: Qt.inputMethod.visible
            PropertyChanges {
                target: inputPanel
                y: itemControl.height - inputPanel.height
                opacity: 1
            }
        }
        transitions: Transition {
            id: inputPanelTransition
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    property: "opacity"
                    duration: 200
                }
            }
        }

        Binding {
            target: InputContext
            property: "animating"
            value: inputPanelTransition.running
        }
    }

    IntValidator {
        id: validatorInteger
    }
    DoubleValidator {
        id: validatorDouble
        notation: DoubleValidator.StandardNotation
        decimals: 6
    }

    Connections {
        target: textValue
        onEditingFinished: {
            console.log("editing finished")
            backend.set_param_value(textValue.text)
        }
    }

    Component.onCompleted: {
        labelTitle.text = backend.param_description
        textValue.text = backend.param_value

        var paramType = backend.param_type
        if (paramType < ConfigBackend.PT_FLOAT) {
            textValue.validator = validatorInteger
        } else if (paramType == ConfigBackend.PT_FLOAT) {
            textValue.validator = validatorDouble
        }
/*
        if (paramType != ConfigBackend.PT_TEXT) {
            InputContext.inputEngine.inputMode = InputEngine.Numeric;
        } else {
            InputContext.inputEngine.inputMode = InputEngine.Latin;
        }*/
    }
}
