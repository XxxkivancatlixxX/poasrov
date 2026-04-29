import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "#081520"

    property bool showPlaceholder: !backend.cameraConnected

    // ── Placeholder ────────────────────────────────────────────────────────
    // Subtle teal-tinted dark background when no feed
    Rectangle {
        anchors.fill: parent
        visible: showPlaceholder
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#0d2235" }
            GradientStop { position: 0.5; color: "#091828" }
            GradientStop { position: 1.0; color: "#061015" }
        }

        // Grid lines — subtle
        Repeater {
            model: 8
            Rectangle {
                x: 0; y: index * (parent.height / 8)
                width: parent.width; height: 1
                color: "#11aaccff"
            }
        }
        Repeater {
            model: 12
            Rectangle {
                x: index * (parent.width / 12); y: 0
                width: 1; height: parent.height
                color: "#11aaccff"
            }
        }

        // Centre placeholder content
        Column {
            anchors.centerIn: parent
            spacing: 12

            // Camera icon (glass orb style)
            Rectangle {
                width: 80; height: 80; radius: 40
                anchors.horizontalCenter: parent.horizontalCenter
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#6620aacc" }
                    GradientStop { position: 0.5; color: "#441080aa" }
                    GradientStop { position: 1.0; color: "#22005577" }
                }
                border.color: "#5588ccee"; border.width: 2
                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 3; height: parent.height * 0.45; radius: 36
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#44ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }
                Text {
                    anchors.centerIn: parent; text: "📷"
                    font.pixelSize: 36
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "No Camera Feed"
                color: "#88bbddff"; font.pixelSize: 16; font.bold: true
                style: Text.Raised; styleColor: "#002244"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Waiting for connection..."
                color: "#5588aacc"; font.pixelSize: 12
            }
        }
    }

    // ── Live video image ───────────────────────────────────────────────────
    Image {
        id: videoImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: !showPlaceholder
        cache: false
        asynchronous: false

        Connections {
            target: backend
            function onCameraFrameChanged() {
                videoImage.source = ""
                videoImage.source = "image://camera/frame"
            }
        }
    }
}
