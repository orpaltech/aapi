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
import QtQuick.Controls.Basic
import QtQuick.Controls.Fusion
import QtQuick.Studio.DesignEffects
import aapi
import ru.orpaltech.aapi

Item {
    id: itemParamEnum
    width: 1262
    height: 554

    property ConfigurationViewBackend backend: aapi.view_configuration

    readonly property int optionHeight: 52

    ColumnLayout {
        id: col1
        anchors.fill: parent
        anchors.leftMargin: 48
        anchors.rightMargin: 48
        anchors.bottomMargin: 128
        spacing: 0

        Row {
            id: row1
            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            bottomPadding: 12
            spacing: 0
            Layout.fillWidth: true

            Label {
                id: labelParamTitle
                text: qsTr("<parameter title>")
                font.pointSize: 24
                verticalAlignment: Text.AlignBottom
                wrapMode: Text.WordWrap
                font.bold: true

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
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.fillWidth: true
            spacing: 0

            ComboBox {
                id: cbOptions
                implicitHeight: optionHeight
                textRole: "text"
                valueRole: "value"
                font.pointSize: 28
                width: row2.width

                model: ListModel {
                    id: cbOptionsModel
                }

                contentItem: Text {
                    id: cbOptionsText
                    leftPadding: 0
                    rightPadding: cbOptions.indicator.width + cbOptions.spacing

                    text: cbOptions.displayText
                    font: cbOptions.font
                    color: cbOptions.pressed ? AapiTheme.style.controlFontColor : AapiTheme.style.controlPressedFontColor
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                delegate: ItemDelegate {
                    id: cbOptionsDelegate

                    required property var model
                    required property int index

                    height: optionHeight
                    width: cbOptions.width
                    contentItem: Text {
                        text: cbOptionsDelegate.model[cbOptions.textRole]
                        font: cbOptions.font
                        color: AapiTheme.style.controlFontColor
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: cbOptions.highlightedIndex === index
                }

                popup: Popup {
                    id: cbOptionsPopup
                    width: cbOptions.width
                    height: Math.min(backend.param_num_options, 5) * optionHeight + 2
                    padding: 1

                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: cbOptions.popup.visible ? cbOptions.delegateModel : null
                        currentIndex: cbOptions.highlightedIndex

                        ScrollIndicator.vertical: ScrollIndicator {
                            id: cbOptionsVScroll
                            active: true
                            width: AapiTheme.style.scrollBarWidth
                        }
                    }
                }

                DesignEffect {
                    effects: [
                        DesignInnerShadow {
                        },
                        DesignDropShadow {
                        }
                    ]
                }

                /* When an item is selected, update the backend */
                onActivated: backend.set_param_option(currentIndex)

                Component.onCompleted: {
                    console.log("param enum loaded")
                    labelParamTitle.text = backend.param_description
                    var opt_labels = backend.param_opt_labels
                    var opt_values = backend.param_opt_values
                    var cur_sel = 0
                    cbOptions.model.clear()
                    for (var i = 0; i < backend.param_num_options; i++) {
                        var el = {
                            text: opt_labels[i],
                            value: opt_values[i]
                        };
                        cbOptions.model.append(el);
                        if (backend.param_value == opt_values[i]) {
                            cur_sel = i;
                        }
                    }
                    cbOptions.currentIndex = cur_sel;
                }
            }
        }
    }
}
