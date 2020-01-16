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
    title: qsTr("Hello World")

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

            Text {
                x: 10
                text: "Reset"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: match3Model.resetGame()
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
                    property var isCurrent: GridView.isCurrentItem

                    height: view.cellHeight
                    width: view.cellWidth

                    Rectangle {
                        anchors.margins: 5
                        anchors.fill: parent
                        border {
                            color: "black"
                            width: 1
                        }
                        radius: width
                        visible: true
                        color: decoration

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("click");
                            }
                        }
                    }
                }

                 move: Transition {
                    SequentialAnimation {
                        NumberAnimation { properties: "x,y"; duration: 400;}
                    }
                }
            }
        }
    }
}
