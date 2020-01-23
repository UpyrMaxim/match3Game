import QtQuick 2.0

Rectangle {

    property var textProp: "Score: 0"
    property int xPosition: 0
    property int yPosition: 10

    y: yPosition
    x: xPosition

    width: 100
    height: 20
    radius: 5

    Text {
        x: 10
        text: textProp
    }
}
