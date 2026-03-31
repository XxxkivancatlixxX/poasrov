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
                            text: "127.0.0.1"
                            
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
                        onClicked: backend.connectTcp(tcpHost.text, parseInt(tcpPort.text))
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
                        onClicked: backend.connectUdp(udpHost.text, parseInt(udpPort.text))
                    }
                }
            }

            // Disconnect button
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#ffffff" }
                    GradientStop { position: 1.0; color: "#f0f8ff" }
                }
                
                border.color: "#f44336"
                border.width: 2
                radius: 6

                Button {
                    anchors.centerIn: parent
                    text: "Disconnect"
                    width: 180
                    height: 50
                    enabled: backend.mavlinkReady
                    
                    background: Rectangle {
                        radius: 8
                        
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: parent.parent.enabled ? (parent.parent.down ? "#e57373" : (parent.parent.hovered ? "#ef5350" : "#f44336")) : "#cccccc" }
                            GradientStop { position: 0.5; color: parent.parent.enabled ? (parent.parent.down ? "#d32f2f" : (parent.parent.hovered ? "#e53935" : "#e53935")) : "#aaaaaa" }
                            GradientStop { position: 1.0; color: parent.parent.enabled ? (parent.parent.down ? "#c62828" : (parent.parent.hovered ? "#d32f2f" : "#c62828")) : "#999999" }
                        }
                        
                        border.color: parent.parent.enabled ? "#b71c1c" : "#666666"
                        border.width: 2
                        
                        // Glass shine
                        Rectangle {
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 2
                            height: 20
                            radius: 6
                            
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#aaffffff" }
                                GradientStop { position: 1.0; color: "transparent" }
                            }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: parent.enabled ? "white" : "#666666"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: backend.disconnectLink()
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
