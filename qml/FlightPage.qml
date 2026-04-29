import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    anchors.fill: parent

    // Full-screen camera feed
    CameraView {
        id: cameraView
        anchors.fill: parent
    }

    // Top shadow vignette so HUD is readable
    Rectangle {
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
        height: 70
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#88000000" }
            GradientStop { position: 1.0; color: "#00000000" }
        }
    }
    // Bottom vignette
    Rectangle {
        anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right
        height: 80
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00000000" }
            GradientStop { position: 1.0; color: "#77000000" }
        }
    }

    // ── HUD top row ────────────────────────────────────────────────────────
    Row {
        anchors.top: parent.top; anchors.left: parent.left
        anchors.margins: 10
        spacing: 7

        // ARMED / DISARMED pill
        Rectangle {
            height: 30
            width: armText.implicitWidth + 24
            radius: 15
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: backend.armed ? "#88ff5050" : "#77ffaa40" }
                GradientStop { position: 0.5; color: backend.armed ? "#77dd2020" : "#6699cc30" }
                GradientStop { position: 1.0; color: backend.armed ? "#55bb1010" : "#44669920" }
            }
            border.color: backend.armed ? "#88ff8888" : "#88ffffff"
            border.width: 1
            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                anchors.margins: 1; height: parent.height * 0.5; radius: 13
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
            Text {
                id: armText
                anchors.centerIn: parent
                text: backend.armed ? "● ARMED" : "● DISARMED"
                color: "white"; font.pixelSize: 11; font.bold: true
                style: Text.Raised
                styleColor: backend.armed ? "#991010" : "#004466"
            }
        }

        // Attitude pills
        Repeater {
            model: [
                { label: "ROLL",  value: backend.roll  },
                { label: "PITCH", value: backend.pitch },
                { label: "YAW",   value: backend.yaw   }
            ]
            Rectangle {
                height: 30; width: 62; radius: 15
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#7750aaff" }
                    GradientStop { position: 0.5; color: "#661488ee" }
                    GradientStop { position: 1.0; color: "#440055bb" }
                }
                border.color: "#77aaddff"; border.width: 1
                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 1; height: parent.height * 0.5; radius: 13
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#44ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }
                Column {
                    anchors.centerIn: parent; spacing: 1
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.label
                        color: "#aaddff"; font.pixelSize: 7; font.bold: true
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.value.toFixed(1) + "°"
                        color: "white"; font.pixelSize: 12; font.bold: true
                        style: Text.Raised; styleColor: "#003366"
                    }
                }
            }
        }

        // Depth pill
        Rectangle {
            height: 30; width: 72; radius: 15
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#7700ccdd" }
                GradientStop { position: 0.5; color: "#5500aacc" }
                GradientStop { position: 1.0; color: "#330077aa" }
            }
            border.color: "#7700ddee"; border.width: 1
            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                anchors.margins: 1; height: parent.height * 0.5; radius: 13
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
            Column {
                anchors.centerIn: parent; spacing: 1
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "DEPTH"
                    color: "#aaffee"; font.pixelSize: 7; font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: backend.depth.toFixed(2) + " m"
                    color: "white"; font.pixelSize: 12; font.bold: true
                    style: Text.Raised; styleColor: "#003344"
                }
            }
        }
    }

    // ── LIVE / OFFLINE camera badge ────────────────────────────────────────
    Rectangle {
        anchors.top: parent.top; anchors.right: parent.right
        anchors.margins: 10
        height: 26; width: liveBadgeText.implicitWidth + 20
        radius: 13
        color: "transparent"
        gradient: Gradient {
            GradientStop { position: 0.0; color: backend.cameraConnected ? "#7730dd60" : "#77dd3030" }
            GradientStop { position: 0.5; color: backend.cameraConnected ? "#5515bb40" : "#55bb1515" }
            GradientStop { position: 1.0; color: backend.cameraConnected ? "#33109930" : "#33991010" }
        }
        border.color: backend.cameraConnected ? "#8860ee88" : "#88ee6060"
        border.width: 1
        Rectangle {
            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
            anchors.margins: 1; height: parent.height * 0.5; radius: 11
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#44ffffff" }
                GradientStop { position: 1.0; color: "#00ffffff" }
            }
        }
        Text {
            id: liveBadgeText
            anchors.centerIn: parent
            text: backend.cameraConnected ? "● LIVE" : "● OFFLINE"
            color: "white"; font.pixelSize: 10; font.bold: true
            style: Text.Raised
            styleColor: backend.cameraConnected ? "#006620" : "#881010"
        }
    }

    // ── ARM / DISARM buttons — bottom left ─────────────────────────────────
    Row {
        anchors.left: parent.left; anchors.bottom: parent.bottom
        anchors.margins: 14
        spacing: 8

        // ARM / DISARM
        Button {
            width: 110; height: 34
            enabled: backend.mavlinkReady && !backend.armingInProgress

            background: Rectangle {
                radius: 17
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: backend.armed ? "#8855ff55" : "#8830ee60" }
                    GradientStop { position: 0.5; color: backend.armed ? "#6630dd30" : "#6615cc40" }
                    GradientStop { position: 1.0; color: backend.armed ? "#441dbb1d" : "#440aaa28" }
                }
                border.color: backend.armed ? "#8888ff88" : "#8860ff80"
                border.width: 1
                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 1; height: parent.height * 0.5; radius: 15
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#44ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }
            }
            contentItem: Text {
                text: !parent.enabled ? "WAIT..." : (backend.armed ? "DISARM" : "ARM")
                color: "white"; font.pixelSize: 12; font.bold: true
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                style: Text.Raised; styleColor: "#003316"
            }
            onClicked: backend.armed ? backend.disarmVehicle() : backend.armVehicle()
        }

        // FORCE ARM
        Button {
            width: 118; height: 34
            visible: !backend.armed
            enabled: backend.mavlinkReady && !backend.armed && !backend.armingInProgress

            background: Rectangle {
                radius: 17
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#88ffcc44" }
                    GradientStop { position: 0.5; color: "#66ee9900" }
                    GradientStop { position: 1.0; color: "#44cc7700" }
                }
                border.color: "#88ffdd44"; border.width: 1
                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 1; height: parent.height * 0.5; radius: 15
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#44ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }
            }
            contentItem: Text {
                text: !parent.enabled ? "WAIT..." : "FORCE ARM"
                color: "white"; font.pixelSize: 12; font.bold: true
                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                style: Text.Raised; styleColor: "#664400"
            }
            onClicked: backend.forceArmVehicle()
        }
    }

    // ── Joystick status panel — bottom right ───────────────────────────────
    Rectangle {
        anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.margins: 14
        width: 180; height: 76
        radius: 12
        color: "transparent"
        gradient: Gradient {
            GradientStop { position: 0.0; color: backend.isJoystickConnected() ? "#7700ccaa" : "#7700aacc" }
            GradientStop { position: 0.5; color: backend.isJoystickConnected() ? "#550099aa" : "#55008899" }
            GradientStop { position: 1.0; color: backend.isJoystickConnected() ? "#33006677" : "#33005577" }
        }
        border.color: backend.isJoystickConnected() ? "#7722ffcc" : "#7722aaee"
        border.width: 1

        Rectangle {
            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
            anchors.margins: 1; height: parent.height * 0.4; radius: 10
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#44ffffff" }
                GradientStop { position: 1.0; color: "#00ffffff" }
            }
        }

        ColumnLayout {
            anchors.fill: parent; anchors.margins: 10; spacing: 4

            RowLayout {
                spacing: 7
                Rectangle {
                    width: 14; height: 14; radius: 7
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: backend.isJoystickConnected() ? "#a0ffa0" : "#ffe090" }
                        GradientStop { position: 0.5; color: backend.isJoystickConnected() ? "#30cc50" : "#ffaa00" }
                        GradientStop { position: 1.0; color: backend.isJoystickConnected() ? "#0a7a28" : "#cc7000" }
                    }
                    border.color: "#ddffffff"; border.width: 1.5
                    Rectangle { x: 1; y: 1; width: 5; height: 4; radius: 2.5; color: "#aaffffff" }
                    SequentialAnimation on opacity {
                        running: backend.isJoystickConnected(); loops: Animation.Infinite
                        NumberAnimation { from: 1.0; to: 0.4; duration: 900 }
                        NumberAnimation { from: 0.4; to: 1.0; duration: 900 }
                    }
                }
                Text {
                    text: "🎮 Controller"
                    color: "white"; font.pixelSize: 12; font.bold: true
                    style: Text.Raised; styleColor: "#003344"
                }
            }
            Text {
                text: backend.isJoystickConnected() ? "✓ Connected" : "✗ Not Connected"
                color: backend.isJoystickConnected() ? "#aaffcc" : "#ffaaaa"
                font.pixelSize: 10
            }
            Text {
                visible: backend.isJoystickConnected() && !backend.armed
                text: "⚠ ARM vehicle to enable"
                color: "#ffdd88"; font.pixelSize: 9
            }
            Text {
                visible: backend.isJoystickConnected() && backend.armed
                text: "✓ Control active"
                color: "#aaffcc"; font.pixelSize: 10; font.bold: true
            }
        }
    }

    // ── Camera controls — bottom centre ────────────────────────────────────
    Row {
        anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 18
        spacing: 12
        visible: backend.cameraConnected

        // Photo button
        Rectangle {
            width: 44; height: 44; radius: 22
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: photoMouse.pressed ? "#9930aaff" : "#7750bbff" }
                GradientStop { position: 0.5; color: photoMouse.pressed ? "#771088ee" : "#550066dd" }
                GradientStop { position: 1.0; color: photoMouse.pressed ? "#550044bb" : "#330033aa" }
            }
            border.color: "#8888ccff"; border.width: 1.5
            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                anchors.margins: 2; height: parent.height * 0.48; radius: 20
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
            Text { anchors.centerIn: parent; text: "📷"; font.pixelSize: 20 }
            MouseArea { id: photoMouse; anchors.fill: parent; onClicked: backend.takePicture() }
        }

        // Record button
        Rectangle {
            width: 44; height: 44; radius: 22
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: backend.isRecording() ? "#99ff4444" : (recordMouse.pressed ? "#99ee3333" : "#77ff5555") }
                GradientStop { position: 0.5; color: backend.isRecording() ? "#77dd2020" : (recordMouse.pressed ? "#77cc1818" : "#55dd2828") }
                GradientStop { position: 1.0; color: backend.isRecording() ? "#55bb1010" : (recordMouse.pressed ? "#55aa0808" : "#33bb1818") }
            }
            border.color: "#88ff8888"; border.width: 1.5
            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                anchors.margins: 2; height: parent.height * 0.48; radius: 20
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
            // Record indicator
            Rectangle {
                anchors.centerIn: parent
                width: backend.isRecording() ? 16 : 22
                height: backend.isRecording() ? 16 : 22
                radius: backend.isRecording() ? 3 : 11
                color: "white"
                Rectangle {
                    visible: !backend.isRecording()
                    x: 2; y: 2; width: 7; height: 6; radius: 4; color: "#aaffffff"
                }
                SequentialAnimation on opacity {
                    running: backend.isRecording(); loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 600 }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 600 }
                }
            }
            MouseArea {
                id: recordMouse; anchors.fill: parent
                onClicked: backend.isRecording() ? backend.stopRecording() : backend.startRecording()
            }
        }
    }
}
