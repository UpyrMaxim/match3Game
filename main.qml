import QtQuick 2.12
import QtQuick.Window 2.12
import custom.Match3Model 1.0
import QtQuick.Dialogs 1.3


Window {
    id: root

    x: { Screen.width / 2 - width / 2 }
    y: { Screen.height / 2 - height / 2 }
    visible: true
    width: 600
    height: width * match3Model.dimentionY / match3Model.dimentionX

    minimumWidth: 300
    minimumHeight: 400

    title: qsTr("Match 3")

    Rectangle {
        id: background

        anchors.fill: parent
        color: "#deb887"

        ResetBtn {
            id: reset

            onClicked: {
                match3Model.resetGame();
            }
        }

        ScoreField {
            id: leftScoreField

            textProp:  "Score: " + match3Model.score
            xPosition: 10
        }

        ScoreField {
            textProp:  "Moves: " + match3Model.moveCounter
            xPosition: leftScoreField.width
        }

        Match3Model {
            id: match3Model
            objectName: "matchModel"

            onGameOver: messageDialog.open()
        }

        GameBoard {
            gameModel: match3Model
        }
    }

    MessageDialog {
        id: messageDialog

        icon: StandardIcon.Information
        title: "Game over!"
        text: "You have no moves left, your score is " + match3Model.score + " for " + match3Model.moveCounter + " moves. \n Do you want to start another one?"
        standardButtons: StandardButton.Yes | StandardButton.No
        visible: false

        onYes: match3Model.resetGame()
        onNo: console.log("presed no")
    }
}
