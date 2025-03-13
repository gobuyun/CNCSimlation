import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1

import "../PublicItem"
import Tool 1.0


Rectangle {
    id: control
    color: Qt.rgba(0.8, 0.8, 0.85)
    property vector2d basePoint
    property bool isPull: true

    readonly property real titleTextSize: 20
    readonly property real normalTextSizee: titleTextSize*0.85

    signal setAxisValReq(var axisPe); // position mode
    signal addAxisValReq(int axisNameVal, int val); // hand mode
    signal startGCodeReq(); // gcode mode
    signal stopGCodeReq();
    // 内部信号
    signal innerUpdateAxisVal(var axisVal);

    x: parent.width - width
    radius: 5

    NumberAnimation on x{
        id: doPushAni
        running: false
        duration: 500
        easing.type: Easing.OutCubic
    }

    NumberAnimation on x{
        id: doPullAni
        running: false
        duration: 500
        easing.type: Easing.OutCubic
    }

    Component.onCompleted: {
        basePoint = Qt.vector2d(x, y);
        doPullAni.to = basePoint.x;
        doPushAni.to = basePoint.x + width*0.9
    }

    function pull() {
        doPullAni.start();
    }

    function push() {
        doPushAni.start();
    }

    function recvAxisVal(axisVal) {
        innerUpdateAxisVal(axisVal);
    }

    function getGCodeData(ps, pe) {
        pe = pe>= gcodeEdit.model.length? gcodeEdit.model.length:pe;
        return gcodeEdit.model.slice(ps, pe);
    }


    function _changedX(v) {
        control.x = v;
    }

    Flickable {
        id: content
        property var axisName: ["x", "y", "z"]

        anchors.fill: parent
        anchors.margins: 1
        ColumnLayout {
            // hand
            Column {
                id: handItem
                width: control.width
                height: handLayout.implicitHeight + handTitle.height

                Rectangle{
                    id: handTitle
                    width: parent.width
                    height: 50
                    radius: 5
                    color: Qt.rgba(control.color.r-0.2, control.color.g-0.2, control.color.b-0.2, 1.0)
                    Text {
                        x: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Hand Mode"
                        color: "white"
                        font.pixelSize: titleTextSize
                    }
                }
                ColumnLayout {
                    id: handLayout
                    spacing: 5
                    readonly property var itemWidthSalt: [0.2, 0.2, 0.3]
                    function handCmd(axixID, val) {
                        addAxisValReq(axixID, val);
                    }

                    Repeater {
                        model: content.axisName
                        delegate: RowLayout {
                            spacing: 2

                            CNCGeneralBtn {
                                font.pixelSize: normalTextSizee
                                text: `${modelData}+`
                                width: control.width * handLayout.itemWidthSalt[0]
                                enabled: !autoCmdBtn.isRunning
                                onPressed: {
                                    handLayout.handCmd(index, 1);
                                }
                                onPressAndHold: {
                                    handLayout.handCmd(index, 1);
                                }
                                onReleased: {
                                    handLayout.handCmd(index, 0);
                                }
                            }
                            CNCGeneralBtn {
                                font.pixelSize: normalTextSizee
                                text: `${modelData}-`
                                width: control.width * handLayout.itemWidthSalt[1]
                                enabled: !autoCmdBtn.isRunning
                                onPressed: {
                                    handLayout.handCmd(index, -1);
                                }
                                onPressAndHold: {
                                    handLayout.handCmd(index, -1);
                                }
                                onReleased: {
                                    handLayout.handCmd(index, 0);
                                }
                            }
                            Rectangle {
                                width: control.width * handLayout.itemWidthSalt[2]
                                height: 50
                                color: Qt.rgba(0.9, 0.9, 0.9, 1)
                                clip:true
                                Text {
                                    id: axisValText
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width
                                    x: parent.width *0.1
                                    font.pixelSize: normalTextSizee
                                    text: "0"
                                    Connections {
                                        target: control
                                        function onInnerUpdateAxisVal(axisVal) {
                                            const axisPos = axisVal[index];
                                            axisValText.text = axisPos;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } // hand end

            // position
            Column {
                id: positionItem
                width: control.width
                height: positionLayout.implicitHeight + positionTitle.height

                Rectangle{
                    id: positionTitle
                    width: parent.width
                    height: 50
                    radius: 5
                    color: Qt.rgba(control.color.r-0.2, control.color.g-0.2, control.color.b-0.2, 1.0)
                    Text {
                        x: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Position Mode"
                        color: "white"
                        font.pixelSize: titleTextSize
                    }
                }

                RowLayout {
                    id: positionLayout
                    readonly property var itemWidthSalt: [0.2, 0.2, 0.2, 0.4]
                    property var targetPos: [0, 0, 0]
                    Repeater {
                        model: content.axisName
                        delegate: ColumnLayout {
                            Item {
                                width: control.width * positionLayout.itemWidthSalt[index]
                                height: 50
                                Text {
                                    // anchors.centerIn: parent
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.bottom
                                    text: modelData
                                    color: "black"
                                    font.pixelSize: titleTextSize
                                }
                            }
                            Rectangle {
                                width: control.width * positionLayout.itemWidthSalt[index]
                                clip: true
                                height: 50
                                TextInput {
                                    font.pixelSize: normalTextSizee
                                    height: parent.height
                                    width: parent.width
                                    autoScroll: true
                                    text: "0"
                                    x: 5
                                    y: height/4
                                    onEditingFinished: {
                                        positionLayout.targetPos[index] = Number(text);
                                    }
                                }
                            }
                        }
                    }
                    CNCGeneralBtn {
                        font.pixelSize: normalTextSizee
                        text: "Run"
                        width: control.width * positionLayout.itemWidthSalt[3]
                        height: 100
                        enabled: !autoCmdBtn.isRunning
                        onClicked: {
                            setAxisValReq(positionLayout.targetPos);
                        }
                    }
                }
            } // position end

            // gcode
            Column {
                id: gcoeItem
                // property int runLine: 0

                width: control.width
                height: gcodeLayout.implicitHeight + gcodeTitle.height

                Rectangle{
                    id: gcodeTitle
                    width: parent.width
                    height: 50
                    radius: 5
                    color: Qt.rgba(control.color.r-0.2, control.color.g-0.2, control.color.b-0.2, 1.0)
                    Text {
                        x: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: "GCode Mode"
                        color: "white"
                        font.pixelSize: titleTextSize
                    }
                }

                RowLayout {
                    id: gcodeLayout
                    readonly property var itemWidthSalt: [0.2, 0.2]

                    CNCGeneralBtn {
                        font.pixelSize: normalTextSizee
                        text: "Select GCode"
                        width: control.width * positionLayout.itemWidthSalt[0]
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                    CNCGeneralBtn {
                        id: autoCmdBtn
                        property bool isRunning: false
                        font.pixelSize: normalTextSizee
                        text: isRunning? "Stop":"Run"
                        width: control.width * positionLayout.itemWidthSalt[1]
                        enabled: currentFilePath.text.length !== 0
                        onClicked: {
                            isRunning = !isRunning;
                            if (isRunning) {
                                control.startGCodeReq();
                            }
                            else
                                control.stopGCodeReq()
                        }
                    }
                }
                Item {
                    width: control.width
                    height: 50
                    clip:true
                    Text {
                        id: currentFilePath
                        width: parent.width
                        anchors.centerIn: parent
                        elide: Text.ElideMiddle
                        text: fileDialog.file
                        color: "black"
                        font.pixelSize: titleTextSize
                    }
                }
                Rectangle {
                    width: control.width
                    height: 330
                    ListView {
                        id: gcodeEdit
                        anchors.fill: parent
                        clip: true
                        delegate: Row {
                            spacing: 5
                            Text {
                                color: "blue"
                                text: `N${index}`
                                anchors.verticalCenter: parent.verticalCenter
                                font.pixelSize: normalTextSizee
                            }
                            Text {
                                color: "black"
                                text: modelData
                                anchors.verticalCenter: parent.verticalCenter
                                font.pixelSize: normalTextSizee
                            }
                        }
                    }
                }
            } // gcode end

            // Common
            // file dialog
            FileDialog {
                id: fileDialog
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                nameFilters: ["gcode files (*.gcode), txt files (*.txt)"]
            }
            FileReader {
                id: fileReader
                file: fileDialog.file
                onSourceChanged: {
                    gcodeEdit.model = source.split("\n");
                }
            }
        }
    }
}
