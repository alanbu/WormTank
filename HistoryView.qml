import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: historyWindow
    visible: true
    width: 330
    height: 210

    TableView
    {
        anchors.fill: parent
        TableViewColumn {
                role: "ticks"
                title: "Ticks"
                width: 50
                horizontalAlignment: Text.AlignHCenter
            }

        TableViewColumn {
                role: "foodLeft"
                title: "Food Left"
                width: 75
                horizontalAlignment: Text.AlignHCenter
        }

        TableViewColumn {
                role: "maxEnergy"
                title: "Max Energy"
                width: 75
                horizontalAlignment: Text.AlignHCenter
        }

        TableViewColumn {
                role: "leaderName"
                title: "Leader"
                width: 75
                horizontalAlignment: Text.AlignHCenter
        }

        TableViewColumn {
            role : "leaderColour"
            title: "Colour"
            width: 50
            delegate: Rectangle {
                width: 50
                color: styleData.value
                border.color: "black"
                border.width: 2
            }
        }

        model: wormTank.history
    }
}
