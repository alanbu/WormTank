import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Item {
    id: wormListWindow
    visible: true
    width: 500
    height: 150

    TableView
    {

        TableViewColumn {
                role: "name"
                title: "Worm Name"
                width: 240
            }

	    TableViewColumn {
		    role : "colourName"
			title: "Position"
			width: 100
		}
        TableViewColumn {
            role : "colour"
            title: "Colour"
            width: 60
            delegate: Rectangle {
                width: 60
                height: 20
                color: styleData.value
                border.color: "black"
                border.width: 2
            }
        }

        TableViewColumn {
            role: "count"
            title: "Left"
            width: 60
        }

        model: worms
        anchors.fill: parent
    }
}

