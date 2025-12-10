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
    id: swpAboutApp
    title: qsTr("About")
    backend: aapi.view_about

    ColumnLayout {
        id: colMain

        Label {
            id: labelTitle
            text: qsTr("Application Information")
            font.pointSize: 16
            Layout.leftMargin: 9
        }

        Frame {
            id: frmContent
            contentHeight: 607
            contentWidth: 1262

            ColumnLayout {
                id: col1
                anchors.fill: parent
                spacing: 0


                RowLayout {
                    id: row1
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    spacing: 12

                    ColumnLayout {
                        id: col11
                        spacing: 0

                        Frame {
                            id: frmDetails
                            padding: 12
                            contentHeight: 496
                            contentWidth: 601

                            background: Rectangle {
                                color: AapiTheme.style.textAreaColor
                                border.color: AapiTheme.style.controlBorderColor
                                radius: 2
                            }

                            ColumnLayout {
                                id: col111
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                spacing: 8


                                Label {
                                    id: lblAppName
                                    text: Application.displayName
                                    font.bold: true
                                    font.pointSize: 24

                                    DesignEffect {
                                        effects: [
                                            DesignDropShadow {
                                            }
                                        ]
                                    }
                                }
                                Label {
                                    id: lblAppVersion
                                    text: qsTr("Ver ") + Application.version
                                    font.bold: true
                                    font.pointSize: 18

                                    DesignEffect {
                                        effects: [
                                            DesignDropShadow {
                                            }
                                        ]
                                    }
                                }
                                Label {
                                    id: lblAppFor
                                    text: qsTr("For Embedded Linux")
                                    font.pointSize: 16

                                    DesignEffect {
                                        effects: [
                                            DesignDropShadow {
                                            }
                                        ]
                                    }
                                }
                                Label {
                                    id: lblCopyright
                                    text: qsTr("Copyright (C) 2013-2025 ORPAL Technology, Inc.")
                                    Layout.topMargin: 20
                                    font.pointSize: 18
                                }
                                Label {
                                    id: lblSystem
                                    text: qsTr("System Information:")
                                    Layout.topMargin: 32
                                    font.pointSize: 16
                                }
                                TextArea {
                                    id: txtSystem
                                    Layout.fillWidth: true
                                    font.pointSize: 16
                                    text: backend.system_information
                                    wrapMode: Text.WordWrap
                                    placeholderText: qsTr("System Information")
                                    implicitHeight: 252

                                    background: Rectangle {
                                        /*color: AapiTheme.style.textAreaColor*/
                                        border.color: AapiTheme.style.controlBorderColor
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        id: col13
                        spacing: 0

                        RowLayout {
                            id: row121
                            spacing: 0

                            ScrollView {
                                id: svLicense
                                contentHeight: 520
                                contentWidth: 625

                                ScrollBar.vertical: ScrollBar {
                                    interactive: true // Enable interaction with the scrollbar
                                    active: true
                                }

                                TextArea {
                                    id: txtLicense
                                    text: aapi.license_text
                                    clip: true
                                    readOnly: true
                                    placeholderText: qsTr("License Text")
                                    wrapMode: Text.Wrap
                                    antialiasing: true
                                    font.pointSize: 18
                                    textFormat: Text.PlainText
                                    // Enable word wrapping
                                    // Match the width of the ScrollView
                                    width: parent.width
                                    height: contentHeight

                                    background: Rectangle {
                                        color: AapiTheme.style.textAreaColor
                                        border.color: AapiTheme.style.controlBorderColor
                                    }
                                }
                            }
                        }
                    }
                }
                RowLayout {
                    id: row2
                    Layout.fillWidth: true
                    spacing: 20

                    ColumnLayout {
                        id: col21
                        spacing: 0

                        Button {
                            id: btnRestart
                            text: qsTr("Restart")
                            font.bold: true
                            rightPadding: 12
                            leftPadding: 12
                            font.pointSize: 18
                            onClicked: {
                                backend.reboot()
                            }
                        }
                    }

                    ColumnLayout {
                        id: col22
                        spacing: 0

                        Button {
                            id: btnExit
                            text: qsTr("Quit")
                            font.bold: true
                            font.italic: false
                            rightPadding: 12
                            leftPadding: 12
                            font.pointSize: 18
                            onClicked: {
                                backend.quit()
                            }
                        }
                    }
                }
            }
        }
    }

}
