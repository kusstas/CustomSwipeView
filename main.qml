import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Custom.Controls 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Swipe View")

    AdvancedSwipeView {
        id: view
        anchors.fill: parent
        currentIndex: 0
        loop: true
        focus: true

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

        Keys.onDigit0Pressed: {
            setCurrentIndex(0)
        }

        Keys.onDigit1Pressed: {
            setCurrentIndex(1)
        }

        Keys.onDigit2Pressed: {
            setCurrentIndex(2)
        }

        Keys.onDigit3Pressed: {
            setCurrentIndex(3)
        }


        Keys.onTabPressed: loop = !loop
        Keys.onSpacePressed: {
            if (orientation == Qt.Vertical)
                orientation = Qt.Horizontal
            else orientation = Qt.Vertical
        }

        Keys.onLeftPressed: prev()
        Keys.onRightPressed: next()

        onSnap: console.log("snap")
    }
}
