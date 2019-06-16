import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 700
    height: 540
    title: qsTr("Worm Tank - ") + wormTank.name
    onClosing: wormTankModelView.exiting();

    Action {
        id: startAction
        text: qsTr("Start")
        onTriggered: {
            var component = Qt.createComponent("StartTank.qml");
            var window = component.createObject(applicationWindow)
        }
    }

    Action {
        id: battleAction
        text: qsTr("Battle")
        onTriggered: {
            var component = Qt.createComponent("Battle.qml");
            var window = component.createObject(applicationWindow)
        }
    }

    Action {
        id: arenaAction
        text: qsTr("Arena")
        onTriggered: wormTankModelView.startArena()
    }

    Action {
        id: leagueAction
        text: qsTr("League")
        onTriggered: wormTankModelView.startLeague()
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Run")
            MenuItem { action: startAction }
            MenuItem { action: battleAction }
            MenuItem { action: arenaAction }
            MenuItem { action: leagueAction }
            MenuItem {
                text: qsTr("&Save")
                onTriggered: wormTank.save();
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: wormTankModelView.exiting();
            }
        }
        Menu {
            title: qsTr("View")
            MenuItem {
                text : qsTr("&Worm List")
                onTriggered: wormTankModelView.wormList()
            }        
            MenuItem {
                text : qsTr("&League Table")
                onTriggered: wormTankModelView.viewLeague()
            }

            MenuItem {
                text : qsTr("&History")
                onTriggered: wormTankModelView.viewHistory()
            }
        }
        Menu {
            title: qsTr("Help")
            MenuItem {
                text: qsTr("&Index")
                onTriggered: wormTankModelView.showHelp();
            }
        }
    }

    MainForm {
        anchors.fill: parent
    }

    statusBar: StatusBar {
            RowLayout {
                Label { text: wormTankModelView.statusText }
            }
        }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}

