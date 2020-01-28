import QtQuick 2.0


Rectangle {
    id: gameBoard

    property var gameModel: ({ })
    property int moveSpeed: 2500
    property int addSpeed: 4000
    property int removeSpeed: 2500

    anchors {
        fill: parent
        margins: 10
        topMargin: 40
    }

    color: "#ffe4c4"

    Timer {
        id: timer

        function setDelayCompletedAddValue (delayTime) {
            timer.restart();
            timer.interval = delayTime;
            timer.start();
        }

        onTriggered: {
            view.addIsFinished = true;
        }
    }

    GridView {
        id: view

        property bool moveSwapIsCompleted: false
        property bool addIsFinished: true

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

                anchors.margins: 4
                anchors.fill: parent

                border {
                    color: "black"
                    width: 2
                }

                radius: (width + height) / 2
                color: decoration

                Text {
                    x: parent.width / 2 - 5
                    y: parent.height / 2 - 5
                    text: display
                }
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
                   id: moveAnimation
                   properties: "x,y"
                   duration: gameBoard.moveSpeed
                   alwaysRunToEnd: true
               }
               ScriptAction {
                   script: {
                       if (!view.moveSwapIsCompleted) {
                           gameModel.removeCells();
                           view.moveSwapIsCompleted = true;
                       }
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
                    script: {
                        if ( view.addIsFinished ) {
                            gameModel.removeCells();
                            view.addIsFinished = false;
                            timer.setDelayCompletedAddValue(gameBoard.addSpeed / 2);
                        }
                    }
                }
            }
        }

        remove: Transition {
            NumberAnimation { property: "scale"; from: 1.0; to: 0; duration: gameBoard.removeSpeed }
        }


        Component.onCompleted: { view.currentIndex = -1 }
    }
}
