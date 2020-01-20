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

        Rectangle {
            y: 10
            x: parent.width - 20 - width
            width: 60
            height: 20
            radius: height / 2
            color: resetButtonMouse.containsMouse ? "burlywood" : "blanchedalmond"
            border {
                color: "black"
                width: 1
            }

            Text {
                x: 10
                text: "Reset"
            }

            MouseArea {
                id: resetButtonMouse

                hoverEnabled: true
                anchors.fill: parent

                onClicked: match3Model.resetGame()
            }
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
