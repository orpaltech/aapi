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
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects
import aapi

SwipePage {
    id: swpMeasurement
    backend: aapi.view_measurement

    property alias labelError: labelError
    property alias btnErrorClose: btnErrorClose
    property alias popupError: popupError

    transformOrigin: Item.TopLeft

    Popup {
        id: popupError
        x: parent.width/2 - width/2
        y: parent.height/2 - height/2
        padding: 10
        modal: true
        focus: true
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnPressOutsideParent | Popup.CloseOnEscape

        ColumnLayout {
            anchors.fill: parent
            Label {
                id: labelError
                wrapMode: Text.WordWrap
                text: "ERROR MESSAGE"
                color: "#e51400"
            }

            Button {
                id: btnErrorClose
                text: "OK"
            }
        }
    }

    Column {
        id: col1
        width: 1280
        height: 656
        transformOrigin: Item.TopLeft

        Row {
            id: row1
            width: 1280
            height: 656
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
                    transformOrigin: Item.TopLeft
                }

                Button {
                    id: btnDown3
                    text: "<<<"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }

                    /*contentItem: Text {
                        text: parent.text
                        font: parent.font
                        opacity: enabled ? 1.0 : 0.3
                        color: parent.down ? "#17a81a" : "#21be2b"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        transformOrigin: Item.TopLeft
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        implicitWidth: 100
                        opacity: enabled ? 1 : 0.3
                        border.color: parent.down ? "#17a81a" : "#21be2b"
                        border.width: 1
                        radius: 2
                    }*/
                }

                Button {
                    id: btnDown2
                    text: "<<"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnDown1
                    text: "<"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                TextField {
                    id: textFreq
                    text: "16000 KHz"
                    verticalAlignment: Text.AlignTop
                    Layout.fillWidth: true
                    passwordCharacter: "*"
                    placeholderText: qsTr("Frequency")
                    transformOrigin: Item.TopLeft
                    font.pointSize: 24

                    DesignEffect {
                        effects: [
                            DesignInnerShadow {
                            },
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnUp3
                    text: ">>>"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnUp2
                    text: ">>"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }

                Button {
                    id: btnUp1
                    text: ">"
                    Layout.preferredWidth: 120
                    font.bold: false
                    font.pointSize: 24
                    transformOrigin: Item.TopLeft
                    autoRepeat: true

                    DesignEffect {
                        effects: [
                            DesignDropShadow {
                            }
                        ]
                    }
                }
            }
        }

        Pane {
            id: pane1
            anchors.fill: parent
            transformOrigin: Item.TopLeft

            GroupBox {
                id: groupBox1
                x: 0
                y: 0
                width: 1262
                height: 638
                font.underline: false
                font.bold: true
                font.italic: true
                font.pointSize: 18
                transformOrigin: Item.TopLeft
                title: qsTr("Measure Frequency")
            }
        }
    }

}
