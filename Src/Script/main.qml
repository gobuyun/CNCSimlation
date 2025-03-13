import QtQuick.Window 2.13
import QtQuick 2.15
import QtQuick.Controls 2.15

import "PublicItem"
import "Page"

import OpenglExt 1.0

Window {
    id: windows

    visible: true
    height: Screen.desktopAvailableHeight
    width: Screen.desktopAvailableWidth

    Component.onCompleted: {
        // showMaximized();
    }

    Timer {
        interval: 10
        repeat: true
        running: true
        onTriggered: {
            openglItem.updateCurAxisVal();
        }
    }

    OpenglItem {
        id: openglItem
        anchors.fill: parent
        focus: true
        mirrorVertically: true
        onCurAxisValChanged: {
            controlPane.recvAxisVal(curAxisVal);
        }
    }

    MouseArea {
        property int lastPosX: 0
        property int lastPosY: 0

        readonly property int moveEvent : 0
        readonly property int rotateEvent : 1
        readonly property int wheelEvent : 2

        function updateLastPos() {
            lastPosX = mouseX;
            lastPosY = mouseY;
        }

        anchors.fill: openglItem
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(event){
            updateLastPos();
        }
        onPositionChanged: function(event){
            openglItem.mouseCmd(event.buttons === Qt.LeftButton?1:0, lastPosX - mouseX, mouseY - lastPosY);
            updateLastPos();
        }
        onWheel: function(event){
            openglItem.mouseCmd(2, event.angleDelta.x, event.angleDelta.y);
        }
    }

    Button {
        property bool isExpand: true
        width: 20
        height: width*2
        anchors.right: controlPane.left
        anchors.top: controlPane.top
        text: isExpand? ">":"<"
        onClicked: {
            isExpand = !isExpand;
            if (isExpand)
                controlPane.pull();
            else
                controlPane.push();
        }
    }
    ControlPopup {
        id: controlPane
        width: 500
        height: Screen.desktopAvailableHeight*0.8
        anchors.verticalCenter: parent.verticalCenter

        onSetAxisValReq: function(axisPe) {
            openglItem.positionCmd(axisPe);
        }
        onAddAxisValReq: function(axisName, val) {
            openglItem.handCmd(axisName, val);
        }
        onStartGCodeReq: {
            autoCmdTimer.start();
        }
        onStopGCodeReq: {
            autoCmdTimer.stop();
            openglItem.stopAutoCmd();
            autoCmdTimer.line = 0;
        }

        Timer {
            id: autoCmdTimer
            property int line: 0
            interval: 1000/60
            running: false
            repeat: true
            onTriggered: {
                if (openglItem.autoRunCanPush()) {
                    const pe = line+1;
                    const axisRuninStr = controlPane.getGCodeData(line, pe);
                    openglItem.autoCmd(axisRuninStr);
                    line = pe;
                }
//                const pe = line+1;
//                const axisRuninStr = controlPane.getGCodeData(line, pe);
//                openglItem.autoCmd(axisRuninStr);
//                line = pe;
            }
        }
    }
}
