import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.orpaltech.aapi 1.0

SwipePage {
    backend: analyzer.measure
    property alias labelError: labelError
    property alias btnErrorClose: btnErrorClose
    property alias popupError: popupError

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



    Label {
        id: labelFreq
        x: 6
        y: 6
        text: qsTr("Frequency:")
        anchors.right: parent.right
        anchors.rightMargin: 366
    }

    TextField {
        id: textFrequency
        x: 226
        y: 0
        width: 109
        height: 26
        text: qsTr("16000 KHz")
        anchors.right: parent.right
        anchors.rightMargin: 121
        topPadding: 4
        bottomPadding: 4
        padding: 12
        font.pointSize: 10
    }

    Button {
        id: button
        x: 337
        y: 0
        height: 25
        text: ">"
        anchors.right: parent.right
        anchors.rightMargin: 87
        font.pixelSize: 12
        autoRepeat: true
    }

    Button {
        id: button1
        x: 370
        y: 0
        height: 25
        text: ">>"
        anchors.right: parent.right
        anchors.rightMargin: 50
        font.pixelSize: 12
        autoRepeat: true
    }

    Button {
        id: button2
        x: 408
        y: 0
        height: 25
        text: ">>>"
        anchors.right: parent.right
        anchors.rightMargin: 2
        font.pixelSize: 12
        autoRepeat: true
    }

    Button {
        id: button3
        x: 193
        y: 0
        height: 25
        text: "<"
        anchors.right: parent.right
        anchors.rightMargin: 231
        font.pixelSize: 12
        autoRepeat: true
    }

    Button {
        id: button4
        x: 155
        y: 0
        height: 25
        text: "<<"
        anchors.right: parent.right
        anchors.rightMargin: 265
        font.pixelSize: 12
        autoRepeat: true
    }

    Button {
        id: button5
        x: 107
        y: 0
        height: 25
        text: "<<<"
        anchors.right: parent.right
        anchors.rightMargin: 303
        font.pixelSize: 12
        autoExclusive: false
        autoRepeat: true
        highlighted: false
    }

}
