import QtQuick 2.4
import QtQuick.Window 2.2

Window {
    id: startWindow
    visible: true
    modality: "ApplicationModal"
    title: qsTr("Start a new worm tank")
    width: 400
    height: 150

    StartTankForm
    {
        id: startForm
        wormTypes.model: wormTankModelView.wormTypes
        okButton.onClicked: {
            wormTankModelView.start(wormTypes.currentText, reset.checked);
            startWindow.destroy();
        }
        cancelButton.onClicked: startWindow.destroy()
    }
}
