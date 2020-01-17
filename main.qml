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
                            console.log(view.selectedIndex, index)
                            let checkValidSwap = Math.abs(view.selectedIndex - index) == 1 || Math.abs(view.selectedIndex - index) == match3Model.dimentionY;
                            if (view.selectedIndex >= 0 && view.selectedIndex != index && checkValidSwap) {
                            console.log('click:')
//                              if (match3Model.choseElement(index)) {
//                                  console.log("asass");
//                                  match3Model.reSwapElements(index);
//                              }
                                let elementIndex = index;
                                var cellsToDestruct = match3Model.swapCells(view.selectedIndex, elementIndex);

                                if (cellsToDestruct.length) {
                                    console.log("in");
                                    while (cellsToDestruct.length) {
                                        // remove elements
                                        cellsToDestruct.forEach(element => match3Model.removeCell(element));
                                        cellsToDestruct = null;
                                        // insert elements

                                        // check board
                                        cellsToDestruct = match3Model.checkBoard();
                                        console.log(cellsToDestruct)
                                    }

                                } else {
                                    match3Model.reSwapCells(view.selectedIndex, elementIndex);
                                }

                                view.selectedIndex = -1;
                            } else {
                               view. selectedIndex = index;
                            }
                        }
                    }

//                    Component.onDestruction: {
//                        destroyAnim.start()
////                        console.log("Destruction:" + index);

//                    }

//                     Behavior on opacity{ NumberAnimation {}}
//                     SequentialAnimation
//                     {
//                         id: destroyAnim
//                           ScriptAction{script: console.log("XD")}
////                         ScriptAction{script: delegateItem.opacity = 0}
////                         NumberAnimation{target: delegateItem; property:"scale"; to: 5}
//                     }
                }
            }
        }
    }
}
