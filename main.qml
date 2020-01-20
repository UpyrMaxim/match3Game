import QtQuick 2.12
import QtQuick.Window 2.12
import custom.Match3Model 1.0


Window {
    id: root

    x: { Screen.width / 2 - width / 2 }
    y: { Screen.height / 2 - height / 2 }
    visible: true
    width: 340
    height: width + 50
    title: qsTr("Match 3")

    Rectangle {
        id: background

        anchors.fill: parent
        color: "#deb887"

        ResetBtn {
            gameModel: match3Model
        }

        ScoreField {
            textProp:  "Score: " + match3Model.score;
            xPosition: 10
        }

        ScoreField {
            textProp:  "Moves: " + match3Model.moveCounter;
             xPosition: 100
        }

        Match3Model {
            id: match3Model
        }
        GameBoard {
            gameModel: match3Model
        }
    }
}
