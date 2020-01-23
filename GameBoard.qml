import QtQuick 2.0


Rectangle {
    id: gameBoard

    property var gameModel: ({ })

    anchors {
        fill: parent
        margins: 10
        topMargin: 40
    }

    color: "#ffe4c4"

    function delay(delayTime, func) {
        timer.restart();
        timer.interval = delayTime;
        timer.handler = func;
        timer.start();
    }

    Timer {
        id: timer

        property var handler: function() { }

        repeat: false

        onTriggered: {
            handler();
        }
    }


    GridView {
        id: view

        property int selectedIndex: -1
        property var cellsToDestruct: []

        function removeCells () {
                view.cellsToDestruct = null;
                view.cellsToDestruct = gameModel.checkBoardCells();
                view.cellsToDestruct.forEach(element => gameModel.removeCell(element));
        }

        anchors.fill: parent
        flow: GridView.FlowTopToBottom
        cellHeight: parent.height / gameModel.dimentionY
        cellWidth: parent.width / gameModel.dimentionX
        model: gameModel
        clip: true
        interactive: false

        delegate: Item {
            id: delegateItem

            height: view.cellHeight
            width: view.cellWidth
            scale: index === view.selectedIndex ? 0.8 : 1

            Rectangle {
                id: cell

                anchors.margins: width / 10
                anchors.fill: parent

                border {
                    color: "black"
                    width: 2
                }

                radius: (width + height) / 2
                visible: true
                color: decoration
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (view.selectedIndex < 0) {
                        view.selectedIndex = index;
                    } else {
                        let indexesDifference = Math.abs(view.selectedIndex - index);
                        let swapIsValid = indexesDifference === 1 || indexesDifference === gameModel.dimentionY;

                        if (swapIsValid) {
                            var elementIndex = index;
                            var prevElementIndex = view.selectedIndex;
                            view.cellsToDestruct = gameModel.swapCells(view.selectedIndex, elementIndex);

                            delay(moveAnimation.duration, function() {
                                if (view.cellsToDestruct.length) {
                                    view.cellsToDestruct.forEach(element => gameModel.removeCell(element));
                                } else {
                                    gameModel.reSwapCells(elementIndex, prevElementIndex);
                                }
                                view.selectedIndex = -1;
                            });
                        } else {
                            view.selectedIndex = index;
                        }
                    }
                }
            }
        }

        move: Transition {
               NumberAnimation { id: moveAnimation; properties: "x,y"; duration: 400; alwaysRunToEnd: true }
        }

        moveDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400 }
        }

        add: Transition {
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 600 }
                    NumberAnimation { id: addAnimation; property: "scale"; easing.type: Easing.OutBounce; from: 0; to: 1.0; duration: 950 }
                    NumberAnimation { properties: "y"; from:  -150 ;  duration: 500 }
                }
                ScriptAction {
                    script: {
                        if (view.cellsToDestruct.length) {
                            view.removeCells();
                        }
                    }
                }
            }
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
