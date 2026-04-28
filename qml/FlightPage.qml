import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    anchors.fill: parent

    // QGC layout: Full screen video with overlay instruments
    CameraView {
        id: cameraView
        anchors.fill: parent
    }

    // QGC top bar overlay
    Rectangle {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#cc000000" }
            GradientStop { position: 1.0; color: "#00000000" }
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 16

            // Armed status (QGC style)
            Rectangle {
                width: 100
                height: 34
                radius: 4
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: backend.armed ? "#cc00aa00" : "#ccaa0000" }
                    GradientStop { position: 1.0; color: backend.armed ? "#8800aa00" : "#88aa0000" }
                }
                
                border.color: backend.armed ? "#00ff00" : "#ff0000"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: backend.armed ? "ARMED" : "DISARMED"
                    color: "white"
                    font.pixelSize: 14
                    font.bold: true
                }
            }

            // Attitude (QGC style)
            Row {
                spacing: 12

                Column {
                    spacing: 2
                    Text {
                        text: "Roll"
                        color: "#aaaaaa"
                        font.pixelSize: 10
                    }
                    Text {
                        text: backend.roll.toFixed(1) + "°"
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                Column {
                    spacing: 2
                    Text {
                        text: "Pitch"
                        color: "#aaaaaa"
                        font.pixelSize: 10
                    }
                    Text {
                        text: backend.pitch.toFixed(1) + "°"
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                Column {
                    spacing: 2
                    Text {
                        text: "Yaw"
                        color: "#aaaaaa"
                        font.pixelSize: 10
                    }
                    Text {
                        text: backend.yaw.toFixed(1) + "°"
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }
            }

            // Depth
            Column {
                spacing: 2
                Text {
                    text: "Depth"
                    color: "#aaaaaa"
                    font.pixelSize: 10
                }
                Text {
                    text: backend.depth.toFixed(2) + " m"
                    color: "#00ddff"
                    font.pixelSize: 16
                    font.bold: true
                }
            }

            Item { Layout.fillWidth: true }

            // Battery (QGC style)
            Row {
                spacing: 8

                Column {
                    spacing: 2
                    Text {
                        text: "Battery"
                        color: "#aaaaaa"
                        font.pixelSize: 10
                    }
                    Text {
                        text: backend.batteryVoltage.toFixed(2) + " V"
                        color: backend.batteryPercent > 20 ? "#00ff00" : "#ffaa00"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                Rectangle {
                    width: 50
                    height: 34
                    color: "#cc000000"
                    border.color: "#666666"
                    radius: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: 2
                        width: (parent.width - 4) * (backend.batteryPercent / 100.0)
                        height: parent.height - 4
                        radius: 2
                        color: backend.batteryPercent > 20 ? "#00ff00" : "#ffaa00"
                    }

                    Text {
                        anchors.centerIn: parent
                        text: backend.batteryPercent + "%"
                        color: "white"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }
    }

    // QGC bottom left: ARM buttons with gloss
    Row {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 16
        spacing: 10
        
        Button {
            width: 140
            height: 60
            text: backend.armed ? "DISARM" : "ARM"
            enabled: backend.mavlinkReady && !backend.armingInProgress

            background: Rectangle {
                radius: 10
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: backend.armed ? "#e57373" : "#81c784" }
                    GradientStop { position: 0.5; color: backend.armed ? "#ef5350" : "#66bb6a" }
                    GradientStop { position: 1.0; color: backend.armed ? "#f44336" : "#4caf50" }
                }
                
                border.color: backend.armed ? "#c62828" : "#2e7d32"
                border.width: 3
                
                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 3
                    height: 22
                    radius: 8
                    
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#aaffffff" }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                }
            }

            contentItem: Text {
                text: parent.enabled ? parent.text : "WAIT..."
                color: "white"
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                style: Text.Raised
                styleColor: backend.armed ? "#b71c1c" : "#1b5e20"
            }

            onClicked: {
                if (backend.armed) {
                    backend.disarmVehicle()
                } else {
                    backend.armVehicle()
                }
            }
        }
        
        Button {
            width: 140
            height: 60
            text: "FORCE ARM"
            enabled: backend.mavlinkReady && !backend.armed && !backend.armingInProgress
            visible: !backend.armed

            background: Rectangle {
                radius: 10
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#ff9800" }
                    GradientStop { position: 0.5; color: "#f57c00" }
                    GradientStop { position: 1.0; color: "#ef6c00" }
                }
                
                border.color: "#e65100"
                border.width: 3
                
                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 3
                    height: 22
                    radius: 8
                    
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#aaffffff" }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                }
            }

            contentItem: Text {
                text: parent.enabled ? parent.text : "WAIT..."
                color: "white"
                font.pixelSize: 14
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                style: Text.Raised
                styleColor: "#bf360c"
            }

            onClicked: backend.forceArmVehicle()
        }
    }
}