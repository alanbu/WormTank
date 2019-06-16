import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

Item {
    property alias wormTypes: wormTypes
    property alias okButton: okButton
    property alias cancelButton: cancelButton
    property alias reset: reset
    width: 400
    height: 140


    GridLayout {
        id: gridLayout1
        x: 8
        y: 0
        width: 378
        height: 130
        columns: 3
        rows: 3

        Text {
            id: text1
            text: qsTr("This is a quick test")
            font.pixelSize: 12
        }

        ComboBox {
            id: wormTypes
            width: 170
            height: 22
            activeFocusOnPress: true
            Layout.column: 1
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        CheckBox {
            id: reset
            text: qsTr("Reset to First Generation")
        }

        Button {
            id: cancelButton
            text: qsTr("Cancel")
            Layout.column: 1
            Layout.row: 2
        }

        Button {
            id: okButton
            text: qsTr("OK")
            isDefault: true
            Layout.column: 2
            Layout.row: 2
        }


    }
}

