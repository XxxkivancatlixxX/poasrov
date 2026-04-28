import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    anchors.fill: parent

    ScrollView {
        anchors.fill: parent
        anchors.margins: 20
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: 20

            Text {
                text: "Communication Links"
                color: "#003c74"
                font.pixelSize: 22
                font.bold: true
            }

            // Camera Settings
            GroupBox {
                Layout.fillWidth: true
                title: "Camera Settings"
                
                background: Rectangle {
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffffff" }
                        GradientStop { position: 1.0; color: "#f0fff0" }
                    }
                    border.color: "#66bb6a"
                    border.width: 2
                    radius: 6
                }
                
                label: Rectangle {
                    color: "#66bb6a"
                    width: cameraLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: cameraLabel
                        text: "Camera Settings"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Text {
                        text: "RTSP Camera URL:"
                        color: "#003c74"
                        font.pixelSize: 12
                    }
                    
                    Text {
                        text: "Common formats: rtsp://IP:PORT/cam, /stream, /video, /live, /h264"
                        color: "#666666"
                        font.pixelSize: 10
                        font.italic: true
                    }

                    TextField {
                        id: cameraUrl
                        Layout.fillWidth: true
                        text: backend.cameraUrl
                        placeholderText: "rtsp://192.168.1.2:8554/cam"
                        
                        background: Rectangle {
                            color: "#ffffff"
                            border.color: cameraUrl.activeFocus ? "#3a7fba" : "#7eb4ea"
                            border.width: 2
                            radius: 4
                        }
                        
                        color: "#003c74"
                        font.pixelSize: 12
                        
                        onTextChanged: {
                            backend.setCameraUrl(text)
                        }
                    }

                    Row {
                        spacing: 10
                        
                        Button {
                            text: "Connect Camera"
                            width: 160
                            height: 40
                            enabled: !backend.cameraConnected
                            
                            background: Rectangle {
                                radius: 6
                                color: parent.enabled ? (parent.down ? "#2e7d32" : (parent.hovered ? "#43a047" : "#4caf50")) : "#cccccc"
                                border.color: parent.enabled ? "#1b5e20" : "#999999"
                                border.width: 2
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 12
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                backend.connectCamera()
                            }
                        }
                        
                        Button {
                            text: "Disconnect Camera"
                            width: 160
                            height: 40
                            enabled: backend.cameraConnected
                            
                            background: Rectangle {
                                radius: 6
                                color: parent.enabled ? (parent.down ? "#c62828" : (parent.hovered ? "#e53935" : "#f44336")) : "#cccccc"
                                border.color: parent.enabled ? "#b71c1c" : "#999999"
                                border.width: 2
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 12
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                backend.disconnectCamera()
                            }
                        }
                    }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        height: 30
                        radius: 4
                        color: backend.cameraConnected ? "#e8f5e9" : "#ffebee"
                        border.color: backend.cameraConnected ? "#4caf50" : "#f44336"
                        border.width: 1
                        
                        Text {
                            anchors.centerIn: parent
                            text: backend.cameraConnected ? "✓ Camera Connected" : "✗ Camera Disconnected"
                            color: backend.cameraConnected ? "#2e7d32" : "#c62828"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                }
            }

            Text {
                text: "Communication Links"
                color: "#003c74"
                font.pixelSize: 22
                font.bold: true
            }

            // TCP Connection
            GroupBox {
                Layout.fillWidth: true
                title: "TCP Connection"
                
                background: Rectangle {
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffffff" }
                        GradientStop { position: 1.0; color: "#f0f8ff" }
                    }
                    border.color: "#5a9fd4"
                    border.width: 2
                    radius: 6
                }
                
                label: Rectangle {
                    color: "#5a9fd4"
                    width: labelText.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: labelText
                        text: "TCP Connection"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 12
                        columnSpacing: 16

                        Text {
                            text: "Host:"
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: tcpHost
                            Layout.fillWidth: true
                            text: "192.168.1.2"
                            
                            background: Rectangle {
                                color: "#ffffff"
                                border.color: tcpHost.activeFocus ? "#3a7fba" : "#7eb4ea"
                                border.width: 2
                                radius: 4
                            }
                            
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Port:"
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: tcpPort
                            Layout.preferredWidth: 120
                            text: "5760"
                            
                            background: Rectangle {
                                color: "#ffffff"
                                border.color: tcpPort.activeFocus ? "#3a7fba" : "#7eb4ea"
                                border.width: 2
                                radius: 4
                            }
                            
                            color: "#003c74"
                            font.pixelSize: 12
                        }
                    }

                    Button {
                        text: "Connect TCP"
                        Layout.preferredWidth: 180
                        Layout.preferredHeight: 40
                        
                        background: Rectangle {
                            radius: 6
                            color: parent.down ? "#2e7d32" : (parent.hovered ? "#43a047" : "#4caf50")
                            border.color: "#1b5e20"
                            border.width: 2
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        onClicked: {
                            console.log("Connecting TCP to", tcpHost.text, tcpPort.text)
                            backend.connectTcp(tcpHost.text, parseInt(tcpPort.text))
                        }
                    }
                }
            }


            // UDP Connection
            GroupBox {
                Layout.fillWidth: true
                title: "UDP Connection"
                
                background: Rectangle {
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffffff" }
                        GradientStop { position: 1.0; color: "#f0f8ff" }
                    }
                    border.color: "#5a9fd4"
                    border.width: 2
                    radius: 6
                }
                
                label: Rectangle {
                    color: "#5a9fd4"
                    width: udpLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: udpLabel
                        text: "UDP Connection"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 12
                        columnSpacing: 16

                        Text {
                            text: "Host:"
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: udpHost
                            Layout.fillWidth: true
                            text: "0.0.0.0"
                            
                            background: Rectangle {
                                color: "#ffffff"
                                border.color: udpHost.activeFocus ? "#3a7fba" : "#7eb4ea"
                                border.width: 2
                                radius: 4
                            }
                            
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Port:"
                            color: "#003c74"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: udpPort
                            Layout.preferredWidth: 120
                            text: "14550"
                            
                            background: Rectangle {
                                color: "#ffffff"
                                border.color: udpPort.activeFocus ? "#3a7fba" : "#7eb4ea"
                                border.width: 2
                                radius: 4
                            }
                            
                            color: "#003c74"
                            font.pixelSize: 12
                        }
                    }

                    Button {
                        text: "Connect UDP"
                        Layout.preferredWidth: 180
                        Layout.preferredHeight: 40
                        
                        background: Rectangle {
                            radius: 6
                            color: parent.down ? "#1565c0" : (parent.hovered ? "#1e88e5" : "#2196f3")
                            border.color: "#0d47a1"
                            border.width: 2
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        onClicked: {
                            console.log("Connecting UDP to", udpHost.text, udpPort.text)
                            backend.connectUdp(udpHost.text, parseInt(udpPort.text))
                        }
                    }
                }
            }

            // Disconnect button
            GroupBox {
                Layout.fillWidth: true
                
                background: Rectangle {
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffffff" }
                        GradientStop { position: 1.0; color: "#ffe0e0" }
                    }
                    border.color: "#f44336"
                    border.width: 2
                    radius: 6
                }
                
                label: Rectangle {
                    color: "#f44336"
                    width: disconnectLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: disconnectLabel
                        text: "Disconnect"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Disconnect Link"
                    width: 200
                    height: 50
                    
                    background: Rectangle {
                        radius: 8
                        color: parent.down ? "#c62828" : (parent.hovered ? "#e53935" : "#f44336")
                        border.color: "#b71c1c"
                        border.width: 2
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: {
                        console.log("Disconnect button clicked")
                        backend.disconnectLink()
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
