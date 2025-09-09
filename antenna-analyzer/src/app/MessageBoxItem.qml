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
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Studio.DesignEffects

Item {
    id: messageBox

    property string iconType: "error"
    property string messageText: qsTr("An error has occurred while initializing audio device")
    property string detailedText: qsTr("Do you want to OK or Cancel ? Please, select.")
    width: 440
    height: 218


    Frame {
        id: frame1
        rightPadding: 17
        leftPadding: 17
        contentHeight: 200
        contentWidth: 406

        ColumnLayout {
            id: col11
            anchors.fill: parent
            spacing: 0

            RowLayout {
                id: row111
                Layout.fillWidth: true
                spacing: 0

                Image {
                    id: icon1
                    width: 64
                    height: 64
                    Layout.rightMargin: 20
                    source: "/images/" + iconType
                    sourceSize {
                        width: 64
                        height: 64
                    }
                }

                Label {
                    id: labelText1
                    text: messageText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    font.pointSize: 16
                }
            }

            RowLayout {
                id: row112
                height: 80
                Layout.fillWidth: true
                spacing: 0

                Label {
                    id: labelText2
                    text: detailedText
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    font.pointSize: 16
                }
            }
        }
    }

}
