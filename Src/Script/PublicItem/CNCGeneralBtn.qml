import QtQuick.Window 2.13
import QtQuick.Controls 2.15

Button {
    id: control
    property color c: "black"
    property real fontSize: 15
    height: 50
    autoRepeat: true
    autoRepeatInterval: 20
    hoverEnabled: false
    // contentItem: Text {
    //     text: control.text
    //     opacity: control.down ? 0.6 : 1.0
    //     color: control.c
    //     // font.pixelSize: control.down? (control.fontSize*0.9):(control.fontSize)
    //     horizontalAlignment: Text.AlignHCenter
    //     verticalAlignment: Text.AlignVCenter
    //     elide: Text.ElideRight
    // }
    background: Rectangle {
        implicitWidth: control.width
        implicitHeight: control.height
        opacity: control.down ? 0.6 : 1.0
        radius: 5
    }
}
