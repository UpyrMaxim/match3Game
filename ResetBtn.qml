import QtQuick 2.0

Rectangle {
    id: button

    property var clicked: function() { }

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

        onClicked: button.clicked()
    }
}
