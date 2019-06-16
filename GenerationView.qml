import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: generationWindow
    visible: true
    width: 295
    height: 250

    TableView
    {
        anchors.fill: parent
        TableViewColumn {
                role: "wormType"
                title: "Worm Type"
                width: 200
            }

        TableViewColumn {
                role: "generation"
                title: "Generation"
                width: 90
                horizontalAlignment: Text.AlignHCenter
        }

        model: wormTankModelView.generationList
    }
}
