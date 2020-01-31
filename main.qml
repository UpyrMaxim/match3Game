import QtQuick 2.12
import QtQuick.Window 2.12
import custom.Match3Model 1.0


Window {
    id: root

    x: { Screen.width / 2 - width / 2 }
    y: { Screen.height / 2 - height / 2 }
    visible: true
    width: 500
    height: width * match3Model.dimentionY / match3Model.dimentionX

    minimumWidth: 300;
    minimumHeight: 400;
    maximumWidth: 2 * height
    maximumHeight: 2 * width

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
        }

        GameBoard {
            gameModel: match3Model
        }
    }
}
