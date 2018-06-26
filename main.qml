import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.2

import Custom.Controls 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Swipe View")

    AdvancedSwipeView {
        id: view
        anchors.centerIn: parent
        width: 300
        height: 300

        Rectangle {
            id: child
            color: "red"
            width: 300
            height: 300
        }


        Component.onCompleted: {
            console.log(count)
            console.log(currentIndex)
            console.log(currentItem)
        }
    }

    MouseArea {
        anchors.fill: view
        onMouseXChanged: {
            child.x = mouseX
            console.log(mouseX)
        }
    }

}
