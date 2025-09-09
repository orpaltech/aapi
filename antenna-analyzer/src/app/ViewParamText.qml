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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi


Item {
    id: itemTextParam
    width: 1262
    height: 554
    opacity: 1

    property ConfigurationViewBackend backend: aapi.view_configuration

    ColumnLayout {
        id: col1
        x: 8
        y: 0
        anchors.fill: parent
        anchors.leftMargin: 48
        anchors.rightMargin: 48
        anchors.bottomMargin: 128
        spacing: 0

        Row {
            id: row1
            bottomPadding: 8
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            spacing: 0
            Layout.fillWidth: true

            Label {
                id: labelParamTitle
                text: "<parameter title>"
                font.pointSize: 24
                verticalAlignment: Text.AlignBottom
                wrapMode: Text.WordWrap
                Layout.bottomMargin: 8
                font.bold: true
                Layout.fillWidth: true

                DesignEffect {
                    effects: [
                        DesignDropShadow {
                        }
                    ]
                }
            }
        }

        Row {
            id: row2
            spacing: 0
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillWidth: true

            TextField {
                id: txtParamValue
                width: 1166
                focus: true
                text: "<parameter value>"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                placeholderText: qsTr("<Enter parameter value>")
                font.pointSize: 28
                Layout.fillWidth: true

                DesignEffect {
                    effects: [
                        DesignInnerShadow {
                        },
                        DesignDropShadow {
                        }
                    ]
                }
            }
        }
    }

    InputPanel {
        id: inputPanel
        y: 554
        z: 99
        anchors.left: parent.left
        anchors.right: parent.right
        scale: 0.84
        state: "hidden"

        states: [
            State {
                name: "hidden"
                when: !Qt.inputMethod.visible
                PropertyChanges {
                    target: inputPanel
                    y: 554
                    opacity: 0
                }
            },
            State {
                name: "visible"
                when: Qt.inputMethod.visible
                PropertyChanges {
                    target: inputPanel
                    y: itemTextParam.height - inputPanel.height * inputPanel.scale
                    opacity: 1
                }
            }
        ]
        transitions: Transition {
            id: inputPanelTransition
            from: "hidden"; to: "visible"
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
        target: txtParamValue
        function onEditingFinished() {
            console.log("editing finished")
            backend.set_param_value(txtParamValue.text)
        }
    }

    Component.onCompleted: {
        console.log("param text loaded")
        labelParamTitle.text = backend.param_description
        txtParamValue.text = backend.param_value

        var paramType = backend.param_type
        if (paramType < ConfigurationViewBackend.PT_FLOAT) {
            txtParamValue.validator = validatorInteger
        } else if (paramType == ConfigurationViewBackend.PT_FLOAT) {
            txtParamValue.validator = validatorDouble
        }

        if (paramType != ConfigurationViewBackend.PT_TEXT) {
            InputContext.inputEngine.inputMode = InputEngine.Numeric;
            txtParamValue.inputMethodHints = Qt.ImhFormattedNumbersOnly;
        } else {
            InputContext.inputEngine.inputMode = InputEngine.Latin;
            txtParamValue.inputMethodHints = Qt.ImhNone;
        }

        //inputPanel.
    }
}
