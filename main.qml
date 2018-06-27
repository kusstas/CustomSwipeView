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
        width: 300
        height: 300
        currentIndex: 1

        Rectangle {
            color: "red"
            width: parent.width
            height: parent.height
        }

        Rectangle {
            color: "green"
            width: parent.width
            height: parent.height
        }

        Rectangle {
            color: "blue"
            width: parent.width
            height: parent.height
        }
    }
}
