/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

Dialog {
    id: dialogRoot

    property alias messageText: customContent.messageText
    property alias detailedText: customContent.detailedText
    property alias messageTextSize: customContent.messageTextSize
    property alias detailedTextSize: customContent.detailedTextSize
    property alias iconType: customContent.iconType
    property alias caption: header.caption

    popupType: Popup.Item
    scale: 1.2

    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

    // Automatic layout fails for some reason, so set the fixed width
    width: 456

    header: Item {
        property alias caption: title.text

        id: header
        implicitHeight: 28

        Rectangle {
            id: background
            anchors.fill: parent
            gradient: Gradient {
                GradientStop {
                    id: headerGradient1
                    color: AapiTheme.style.headerGradientColor1
                    position: 0.0
                }
                GradientStop {
                    id: headerGradient2
                    color: AapiTheme.style.headerGradientColor2
                    position: 1.0
                }
            }
        }

        Label {
            id: title
            text: "Title"
            font.pointSize: 14
            font.bold: true
            anchors.fill: parent
            anchors.leftMargin: 4
            anchors.topMargin: 2
            color: AapiTheme.style.titleFontColor
        }
    }

    contentItem: MessageBoxItem {
        id: customContent
    }

    footer: DialogButtonBox {
        id: buttonBox
        standardButtons: dialogRoot.standardButtons
        alignment: Qt.AlignLeft
        background: Rectangle {
            color: "transparent"
        }
    }
}
