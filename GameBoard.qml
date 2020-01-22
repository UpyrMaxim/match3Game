import QtQuick 2.0


Rectangle {
    id: gameBoard

    property var gameModel: ({})

    y: 40
    x: 10
    width: parent.width - 20
    height: width + 20
    color: "#ffe4c4"

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


    GridView {
        id: view

        property int selectedIndex: -1

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

                    if(view.selectedIndex < 0) {
                        view. selectedIndex = index;
                    } else {
                        let swapIsValid = Math.abs(view.selectedIndex - index) == 1 || Math.abs(view.selectedIndex - index) === gameModel.dimentionY;

                        if (swapIsValid) {
                            var elementIndex = index;
                            var prevElementIndex = view.selectedIndex;
                            var cellsToDestruct = gameModel.swapCells(view.selectedIndex, elementIndex);

                            delay(moveAnimation.duration, function() {
                                if (cellsToDestruct.length) {
                                    while (cellsToDestruct.length) {
                                        cellsToDestruct.forEach(element => gameModel.removeCell(element));
                                        cellsToDestruct = null;
                                        cellsToDestruct = gameModel.checkBoard();
                                    }
                                } else {
                                    gameModel.reSwapCells(elementIndex, prevElementIndex);
                                }
                                view.selectedIndex = -1;
                            })
                        } else {
                            view. selectedIndex = index
                        }
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
