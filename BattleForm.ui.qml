import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2


Item {
    property alias team1: team1
    property alias team2: team2
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
        rows: 4

        Text {
            id: text1
            text: qsTr("First team:")
            font.pixelSize: 12
        }

        ComboBox {
            id: team1
            width: 170
            height: 22
            activeFocusOnPress: true
            Layout.column: 1
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Text {
            id: text2
            text: qsTr("Second team:")
            font.pixelSize: 12
            Layout.row: 2
        }

        ComboBox {
            id: team2
            width: 170
            height: 22
            activeFocusOnPress: true
            Layout.row: 2
            Layout.column: 1
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }


        CheckBox {
            id: reset
            text: qsTr("Use first generations")
        }

        Button {
            id: cancelButton
            text: qsTr("Cancel")
            Layout.column: 1
            Layout.row: 4
        }

        Button {
            id: okButton
            text: qsTr("OK")
            isDefault: true
            Layout.column: 2
            Layout.row: 4
        }

    }
}

