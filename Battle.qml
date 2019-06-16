import QtQuick 2.4
import QtQuick.Window 2.2


Window {
    id: battleWindow
    visible: true
    modality: "ApplicationModal"
    title: qsTr("Start worm battle!")
    width: 400
    height: 150

    BattleForm
    {
        id: battleForm
        team1.model: wormTankModelView.wormTypes
        team2.model: wormTankModelView.wormTypes
        okButton.onClicked: {
            wormTankModelView.startBattle(team1.currentText, team2.currentText, reset.checked);
            battleWindow.destroy();
        }
        cancelButton.onClicked: battleWindow.destroy()
    }
}
