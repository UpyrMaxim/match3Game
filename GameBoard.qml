import QtQuick 2.0


Rectangle {
    id: gameBoard


    property var gameModel: ({ })
    property int moveSpeed: 400
    property int addSpeed: 600
    property int removeSpeed: 400

    anchors {
        fill: parent
        margins: 10
        topMargin: 40
    }

    color: "#ffe4c4"

    GridView {
        id: view

        property var cellsToDestruct: []

        anchors {
            fill: parent
        }

        flow: GridView.FlowTopToBottom
        cellHeight: parent.height / gameModel.dimentionY
        cellWidth: parent.width / gameModel.dimentionX
        model: gameModel
        interactive: false

        delegate: Item {
            id: delegateItem

            height: view.cellHeight
            width: view.cellWidth
            scale: index === gameModel.selectedIndex ? 0.8 : 1

            Rectangle {
                id: cell

                anchors.margins: 4
                anchors.fill: parent

                border {
                    color: "black"
                    width: 2
                }

                radius: (width + height) / 2
                color: decoration
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    view.currentIndex = gameModel.selectedIndex;
                    if (!gameModel.chooseCell(index)) {
                        if (gameModel.selectedIndex < 0) {
                            delegateItem.state = "fail";
                            view.currentItem.state = "fail";
                        }
                    }
                }
            }
            states: State {
                name: "fail";
            }

            transitions: Transition {
                from: "*"
                to: "fail"

                SequentialAnimation {
                    SequentialAnimation {
                        loops: 4

                        NumberAnimation {
                            target: delegateItem
                            property: "scale"
                            from: 1.2
                            to: 1
                            duration: 50
                            easing.type: Easing.InOutBack
                        }

                        NumberAnimation {
                            target: delegateItem
                            property: "scale"
                            from: 0.8
                            to: 1
                            duration: 50
                            easing.type: Easing.InOutBack
                        }
                    }

                    ScriptAction {
                        script: {
                            delegateItem.state = "default";
                        }
                    }
                }
            }
        }

        move: Transition {
            SequentialAnimation {
               NumberAnimation {
                   id: moveAnimation;
                   properties: "x,y";
                   duration: gameBoard.moveSpeed;
                   alwaysRunToEnd: true
               }

               ScriptAction {
                   script: {
                       gameModel.removeCells();
                   }
               }
            }
        }

        moveDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: gameBoard.moveSpeed }
        }

        add: Transition {
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: gameBoard.addSpeed }
                    NumberAnimation { properties: "y"; from:  -(view.height - y); duration: gameBoard.addSpeed }
                }
            }
        }

        addDisplaced: Transition {
            SequentialAnimation {
               NumberAnimation {
                   id: moveOnAdd;
                   properties: "x,y";
                   duration: gameBoard.addSpeed;
                   easing.type: Easing.InBack
               }

               ScriptAction {
                   script:  gameModel.removeCells();
               }
            }
        }

        remove: Transition {
            NumberAnimation { property: "scale"; from: 1.0; to: 0; duration: gameBoard.removeSpeed }
        }
    }
}
