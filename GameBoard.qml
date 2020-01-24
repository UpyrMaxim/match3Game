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

    GridView {
        id: view

        property int selectedIndex: -1
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
            scale: index === view.selectedIndex ? 0.8 : 1

            Rectangle {
                id: cell

                anchors.margins: 4
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
                        if (!gameModel.swapCells(view.selectedIndex, index)) {
                            delegateItem.state = "fail";
                            view.currentIndex = model.index + (view.selectedIndex - index);
                            view.currentItem.state = "fail";
                        }
                        view.selectedIndex = -1;
                    }
                }
            }

            states: State {
                name: "fail";
                PropertyChanges { target: delegateItem; }
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
               NumberAnimation { id: moveAnimation; properties: "x,y"; duration: 400; alwaysRunToEnd: true }
               ScriptAction {
                   script: {
                       gameModel.removeCells();
                   }
               }
            }
        }

        moveDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400 }
        }

        add: Transition {
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 600 }
                    NumberAnimation { properties: "y"; from:  -(view.height - y) ;  duration: 600 }
                }
            }
        }

        addDisplaced: Transition {
            SequentialAnimation {
               NumberAnimation {id: moveOnAdd; properties: "x,y"; duration: 600; easing.type: Easing.InBack }
               ScriptAction {
                   script:  gameModel.removeCells();
               }
            }
        }

        remove: Transition {
            NumberAnimation { property: "scale"; from: 1.0; to: 0; duration: 400 }
        }

        removeDisplaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBack }
        }
    }
}
