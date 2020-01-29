import QtQuick 2.0


Rectangle {
    id: gameBoard

    property var gameModel: ({ })
    property int moveSpeed: 400
    property int addSpeed: 1000
    property int removeSpeed: 900

    anchors {
        fill: parent
        margins: 10
        topMargin: 40
    }

    color: "#ffe4c4"

    GridView {
        id: view

        property bool moveSwapIsCompleted: false
        property int nextIndexToDelete: -1

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
            scale: index === view.currentIndex ? 0.8 : 1

            Rectangle {
                id: cell

                anchors.margins: 2
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
                    view.moveSwapIsCompleted = false;

                    if (view.currentIndex == index) {
                        view.currentIndex = -1;
                    } else if (view.currentIndex < 0 || ![1, gameModel.dimentionY].includes(Math.abs(view.currentIndex - index))) {
                        view.currentIndex = index;
                    } else {
                        if (!gameModel.chooseCell(view.currentIndex, index)) {
                            delegateItem.state = "fail";
                            view.currentItem.state = "fail";
                        }

                        view.currentIndex = -1;
                    }
                }
            }
            states: State {
                name: "fail"
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
                    id: moveAnimation
                    properties: "x,y"
                    duration: gameBoard.moveSpeed
                    alwaysRunToEnd: true
                }

                ScriptAction {
                    script: {
                        if (!view.moveSwapIsCompleted) {
                            view.nextIndexToDelete = gameModel.removeCells();
                            view.moveSwapIsCompleted = true;
                        }
                    }
                }
            }
        }

        moveDisplaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: gameBoard.moveSpeed
            }
        }

        add: Transition {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1.0
                duration: gameBoard.addSpeed
            }

            NumberAnimation {
                properties: "y"
                from: -(view.height - y)
                duration: gameBoard.addSpeed
            }
        }

        addDisplaced: Transition {
            NumberAnimation {
                id: moveOnAdd
                properties: "x,y"
                duration: gameBoard.addSpeed
                easing.type: Easing.InBack
            }
        }

        remove: Transition {
            id: removeAnimation

            NumberAnimation {
                property: "scale"
                from: 1.0
                to: 0
                duration: gameBoard.removeSpeed
            }

            SequentialAnimation {
                PauseAnimation {
                    duration: gameBoard.addSpeed
                }

                ScriptAction {
                    script: {
                        if (view.nextIndexToDelete === removeAnimation.ViewTransition.index) {
                            view.nextIndexToDelete = gameModel.removeCells();
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            view.currentIndex = -1;
        }
    }
}
