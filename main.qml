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

    function delay(delayTime, func) {
        timer.restart();
        timer.interval = delayTime;
        timer.handler = func;
        timer.start();

    }

    Timer {
        id: timer

        property var handler: function() {}

        repeat: false;

        onTriggered: {
            handler();
        }
    }

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

        Rectangle {
            y: 10
            x: 20
            width: 100
            height: 20
            radius: height / 2

            Text {
                x: 10
                text: "Score: " + match3Model.score
            }
        }

        Rectangle {
            y: 10
            x: parent.width / 2 - 40
            width: 100
            height: 20
            radius: height / 2

            Text {
                x: 10
                text: "Moves: " + match3Model.moveCounter
            }
        }


        Rectangle {
            id: gameBoard

            y: 40
            x: 10
            width: parent.width - 20
            height: width + 20
            color: "#ffe4c4"

            GridView {
                id: view

                property int selectedIndex: -1

                anchors.margins: 0
                anchors.fill: parent
                flow: GridView.FlowTopToBottom
                cellHeight: parent.height / match3Model.dimentionY
                cellWidth: parent.width / match3Model.dimentionX
                model: Match3Model {
                    id: match3Model
                }
                clip: true
                interactive: false

                delegate: Item {
                    id: delegateItem

                    height: view.cellHeight
                    width: view.cellWidth

                    Rectangle {
                        id: cell

                        anchors.margins: 5
                        anchors.fill: parent
                        border {
                            color: "black"
                            width: 1
                        }
                        radius: width
                        visible: true
                        color: decoration
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            let swapIsValid = Math.abs(view.selectedIndex - index) == 1 || Math.abs(view.selectedIndex - index) == match3Model.dimentionY;

                            if (view.selectedIndex >= 0 && view.selectedIndex != index && swapIsValid) {
                                var elementIndex = index;
                                var prevElementIndex = view.selectedIndex;
                                var cellsToDestruct = match3Model.swapCells(view.selectedIndex, elementIndex);

                                delay(moveAnimation.duration, function() {
                                    if (cellsToDestruct.length) {
                                        while (cellsToDestruct.length) {
                                            cellsToDestruct.forEach(element => match3Model.removeCell(element));
                                            cellsToDestruct = null;
                                            cellsToDestruct = match3Model.checkBoard();
                                        }
                                    } else {
                                        match3Model.reSwapCells(elementIndex, prevElementIndex);
                                    }
                                    view.selectedIndex = -1;
                                })
                            } else {
                               view. selectedIndex = index;
                            }
                        }
                    }
                }

                move: Transition {
                       NumberAnimation { id: moveAnimation; properties: "x,y"; duration: 400; alwaysRunToEnd: true }
                }
                moveDisplaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 400; }
                }

                add: Transition {
                     NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 600 }
                     NumberAnimation { property: "scale"; easing.type: Easing.OutBounce; from: 0; to: 1.0; duration: 950 }
                     NumberAnimation { properties: "y"; from: 0;  duration: 800; }
                 }

                 addDisplaced: Transition {
                     NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.InBack }
                 }

                 remove: Transition {
                     NumberAnimation { property: "scale"; from: 1.0; to: 0; duration: 400 }
                 }

                 removeDisplaced: Transition {
                     NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBack }
                 }
            }
        }
    }
}
