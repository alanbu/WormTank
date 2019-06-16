import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import WormTank 1.0

Item {
    width: 700
    height: 491

    Label {
        id: text1
        x: 19
        y: 8
        text: qsTr("Generation")
    }

    Text {
        id: text2
        x: 120
        y: 10
        text: wormTank.generation
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
        y: 10
        text: qsTr("Tick")
    }

    Label {
        id: label2
        x: 350
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
        x: 120
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
        x: 350
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
        x: 120
        y: 55
        text: wormTank.maxEnergy
    }

    GroupBox {
        id: groupBox1
        x: 445
        y: 80
        width: 234
        height: 114
        title: qsTr("Leader stats")

        Text {
            id: text3
            x: 6
            y: 6
            text: qsTr("Age")
            font.pointSize: 11
        }

        Text {
            id: text4
            x: 905
            y: 6
            text: wormTank.leaderAge
            font.pixelSize: 12
        }

        Text {
            id: text5
            x: 6
            y: 29
            width: 72
            height: 17
            text: qsTr("Leading for")
        }

        Text {
            id: text6
            x: 108
            y: 29
            text: wormTank.leaderAtTop
        }

        Text {
            id: text14
            x: 6
            y: 49
            text: qsTr("Ancestors")
        }

        ListView {
            id: listView1
            x: 6
            y: 69
            width: 214
            height: 16
            orientation: ListView.Horizontal
            delegate: Item {
                width: 30
                height: 16
                Rectangle {
                    width: 30
                    height: 16
                    color: leaderColour
                    Text {
                        text: leaderName
                        font.pixelSize: 8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
            model: wormTank.history
        }

        Text {
            id: text18
            x: 108
            y: 8
            text: wormTank.leaderAge
            font.pointSize: 11
        }
    }

    GroupBox {
        id: groupBox2
        x: 443
        y: 200
        width: 239
        height: 104
        title: qsTr("Last generation stats")

        Text {
            id: text7
            x: 10
            y: 13
            text: qsTr("Ticks")
        }

        Text {
            id: text8
            x: 112
            y: 13
            text: wormTank.lastGenerationTicks
        }

        Text {
            id: text9
            x: 10
            y: 39
            width: 23
            height: 19
            text: qsTr("Food left")
        }

        Text {
            id: text10
            x: 112
            y: 40
            text: wormTank.lastGenerationFoodLeft
        }

        Text {
            id: text11
            x: 10
            y: 64
            text: qsTr("Max energy")
        }

        Text {
            id: text12
            x: 112
            y: 64
            text: wormTank.lastGenerationMaxEnergy
        }
    }

    TrendChart {
        x: 502
        y: 310
        width: 177
        height: 39
        model: wormTank.history
        role: "ticks"
    }

    Text {
        id: text15
        x: 443
        y: 321
        text: qsTr("Ticks")
    }

    Text {
        id: text16
        x: 443
        y: 370
        text: qsTr("Food")
    }

    TrendChart {
        x: 502
        y: 358
        width: 177
        height: 39
        colour: "#008000"
        role: "foodLeft"
        model: wormTank.history
    }

    TrendChart {
        x: 502
        y: 408
        width: 177
        height: 39
        colour: "#0000ff"
        role: "maxEnergy"
        model: wormTank.history
    }

    Text {
        id: text17
        x: 443
        y: 421
        text: qsTr("Energy")
    }
}

