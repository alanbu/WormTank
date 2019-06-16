import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    width: 700
    height: 491

    Text {
        id: text1
        x: 19
        y: 8
        text: qsTr("Generation")
        font.pixelSize: 12
    }

    Text {
        id: text2
        x: 97
        y: 8
        text: wormTank.generation
        font.pixelSize: 12
    }

    Image {
        id: image1
        x: 19
        y: 80
        width: 400
        height: 400
        source: "image://wormtankframe/" + wormTank.tick

        Rectangle {
            id: rectangle1
            x: 100
            y: 100
            width: 200
            height: 200
            color: "#ffffff"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            visible: wormTankModelView.finishedTextVisible

            Text {
                id: text13
                y: 88
                text: wormTankModelView.finishedText
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                wrapMode: Text.WordWrap
                font.pixelSize: 20
            }
        }

    }

    Label {
        id: label1
        x: 254
        y: 8
        text: qsTr("Tick")
    }

    Label {
        id: label2
        x: 305
        y: 8
        text: wormTank.tick
    }

    Label {
        id: label3
        x: 19
        y: 33
        text: qsTr("Worms Left")
    }

    Label {
        id: label4
        x: 97
        y: 33
        text: wormTank.wormsLeft
    }

    Label {
        id: label5
        x: 254
        y: 33
        text: qsTr("Food Left")
    }

    Label {
        id: label6
        x: 320
        y: 33
        text: wormTank.foodLeft
    }

    Label {
        id: label7
        x: 19
        y: 55
        text: qsTr("Max energy")
    }

    Label {
        id: label8
        x: 97
        y: 55
        text: wormTank.maxEnergy
    }

    GroupBox {
        id: groupBox1
        x: 445
        y: 80
        width: 234
        height: 67
        title: qsTr("Leader stats")

        Text {
            id: text3
            x: 6
            y: 6
            text: qsTr("Age")
            font.pixelSize: 12
        }

        Text {
            id: text4
            x: 90
            y: 6
            text: wormTank.leaderAge
            font.pixelSize: 12
        }

        Text {
            id: text5
            x: 4
            y: 26
            width: 72
            height: 14
            text: qsTr("Leading for")
            font.pixelSize: 12
        }

        Text {
            id: text6
            x: 85
            y: 26
            text: wormTank.leaderAtTop
            font.pixelSize: 12
        }
    }

    GroupBox {
        id: groupBox2
        x: 440
        y: 174
        width: 239
        height: 104
        title: qsTr("Last generation stats")

        Text {
            id: text7
            x: 6
            y: 13
            text: qsTr("Ticks")
            font.pixelSize: 12
        }

        Text {
            id: text8
            x: 88
            y: 13
            text: wormTank.lastGenerationTicks
            font.pixelSize: 12
        }

        Text {
            id: text9
            x: 10
            y: 39
            width: 23
            height: 19
            text: qsTr("Food left")
            font.pixelSize: 12
        }

        Text {
            id: text10
            x: 88
            y: 42
            text: wormTank.lastGenerationFoodLeft
            font.pixelSize: 12
        }

        Text {
            id: text11
            x: 10
            y: 64
            text: qsTr("Max energy")
            font.pixelSize: 12
        }

        Text {
            id: text12
            x: 88
            y: 64
            text: wormTank.lastGenerationMaxEnergy
            font.pixelSize: 12
        }
    }

}

