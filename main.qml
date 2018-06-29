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
        //        width: 300
        //        height: 300
        anchors.fill: parent
        currentIndex: 0
        loop: true
        focus: true
        //orientation: Qt.Vertical

        Rectangle {
            color: "red"
        }

        Rectangle {
            color: "green"
        }

        Rectangle {
            color: "blue"
        }

        Rectangle {
            color: "yellow"
        }

        Keys.onTabPressed: {
            currentIndex = 3
        }

        onSeizedChanged: {
            console.log("seized: ", seized)
        }

        onCurrentIndexChanged: {
            console.log(currentIndex)
        }
    }
}
