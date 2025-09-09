import QtQuick
import QtQuick.Controls

Dialog {
    property MessageBoxItem content: contentItem

    property alias messageText: content.messageText
    property alias detailedText: content.detailedText
    property alias iconType: content.iconType
    property alias caption: header.caption

    id: dialog
    popupType: Popup.Item
    scale: 1.2
    anchors.centerIn: parent
    // automatic layout fails for some reason, so set the fixed width
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
                    id: gradient1;
                    color: AapiTheme.style.barGradientColor1
                    position: 0.0
                }
                GradientStop {
                    id: gradient2;
                    color: AapiTheme.style.barGradientColor2
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
        id: content
    }
}
