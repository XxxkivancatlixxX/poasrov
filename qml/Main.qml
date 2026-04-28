import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 1600
    height: 900
    title: "QGroundControl"
    
    // Frutiger Aero gradient background
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#5a9fd4" }
            GradientStop { position: 0.3; color: "#3a7fba" }
            GradientStop { position: 0.7; color: "#306ba3" }
            GradientStop { position: 1.0; color: "#5a9fd4" }
        }
    }

    // QGC style top toolbar with Frutiger Aero
    header: ToolBar {
        height: 50
        
        background: Rectangle {
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ffffff" }
                GradientStop { position: 0.5; color: "#d6e8f7" }
                GradientStop { position: 1.0; color: "#b3d9f2" }
            }
            
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 2
                color: "#003c74"
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 16

            // QGC Logo with Frutiger Aero style
            Button {
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                
                background: Rectangle {
                    radius: 6
                    
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: parent.parent.down ? "#d6e8f7" : (parent.parent.hovered ? "#e8f4fd" : "#ffffff") }
                        GradientStop { position: 0.5; color: parent.parent.down ? "#b3d9f2" : (parent.parent.hovered ? "#d0e9f9" : "#f0f8ff") }
                        GradientStop { position: 1.0; color: parent.parent.down ? "#90c8ee" : (parent.parent.hovered ? "#b8ddf4" : "#e8f4fd") }
                    }
                    
                    border.color: parent.parent.hovered ? "#5a9fd4" : "#7eb4ea"
                    border.width: 2
                }
                
                contentItem: Row {
                    spacing: 10
                    anchors.centerIn: parent
                    
                    // QGC logo circle
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 16
                        anchors.verticalCenter: parent.verticalCenter
                        
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#7eb4ea" }
                            GradientStop { position: 0.5; color: "#5a9fd4" }
                            GradientStop { position: 1.0; color: "#3a7fba" }
                        }
                        
                        border.color: "#003c74"
                        border.width: 2
                        
                        Text {
                            anchors.centerIn: parent
                            text: "Q"
                            color: "white"
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                    
                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 0
                        
                        Text {
                            text: "PoasControll"
                            color: "#003c74"
                            font.pixelSize: 13
                            font.bold: true
                        }
                        
                        Text {
                            text: "Click for settings"
                            color: "#5a9fd4"
                            font.pixelSize: 9
                        }
                    }
                }
                
                onClicked: settingsDialog.open()
            }

            Item { Layout.fillWidth: true }

            // Connection status (QGC style)
            Column {
                spacing: 2
                Layout.alignment: Qt.AlignVCenter
                
                Text {
                    text: backend.connectionStatus
                    color: "#003c74"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignRight
                }
                
                Text {
                    text: backend.mavlinkReady ? "MAVLink: Ready" : "MAVLink: Waiting..."
                    color: backend.mavlinkReady ? "#2e7d32" : "#e65100"
                    font.pixelSize: 10
                    horizontalAlignment: Text.AlignRight
                }
            }

            // Status indicator
            Rectangle {
                width: 16
                height: 16
                radius: 8
                Layout.alignment: Qt.AlignVCenter
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: backend.mavlinkReady ? "#81c784" : "#ffb74d" }
                    GradientStop { position: 1.0; color: backend.mavlinkReady ? "#4caf50" : "#ff9800" }
                }
                
                border.color: backend.mavlinkReady ? "#2e7d32" : "#e65100"
                border.width: 2
                
                SequentialAnimation on opacity {
                    running: !backend.mavlinkReady
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.3; duration: 600 }
                    NumberAnimation { from: 0.3; to: 1.0; duration: 600 }
                }
            }
        }
    }

    // Main flight view (QGC layout)
    FlightPage { }

    // QGC style settings dialog with Frutiger Aero
    Dialog {
        id: settingsDialog
        modal: true
        x: (parent.width - width) / 2
        y: 40
        width: 1000
        height: 700
        
        background: Rectangle {
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ffffff" }
                GradientStop { position: 1.0; color: "#d6e8f7" }
            }
            
            border.color: "#003c74"
            border.width: 3
            radius: 8
        }

        header: Rectangle {
            height: 60
            
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#5a9fd4" }
                GradientStop { position: 0.5; color: "#3a7fba" }
                GradientStop { position: 1.0; color: "#306ba3" }
            }
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12
                
                Rectangle {
                    width: 36
                    height: 36
                    radius: 18
                    
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#ffffff" }
                        GradientStop { position: 0.5; color: "#e8f4fd" }
                        GradientStop { position: 1.0; color: "#d0e9f9" }
                    }
                    
                    border.color: "#003c74"
                    border.width: 2
                    
                    Text {
                        anchors.centerIn: parent
                        text: "Q"
                        color: "#3a7fba"
                        font.pixelSize: 20
                        font.bold: true
                    }
                }
                
                Text {
                    text: "Settings"
                    color: "white"
                    font.pixelSize: 18
                    font.bold: true
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "×"
                    width: 36
                    height: 36
                    
                    background: Rectangle {
                        radius: 6
                        
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: parent.parent.down ? "#e57373" : (parent.parent.hovered ? "#ef5350" : "#f44336") }
                            GradientStop { position: 1.0; color: parent.parent.down ? "#c62828" : (parent.parent.hovered ? "#d32f2f" : "#c62828") }
                        }
                        
                        border.color: "#b71c1c"
                        border.width: 2
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: settingsDialog.close()
                }
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // Left sidebar (QGC style)
            Rectangle {
                Layout.preferredWidth: 100
                Layout.fillHeight: true
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#e8f4fd" }
                    GradientStop { position: 1.0; color: "#d0e9f9" }
                }
                
                Rectangle {
                    anchors.right: parent.right
                    width: 2
                    height: parent.height
                    color: "#7eb4ea"
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.topMargin: 20
                    spacing: 12

                    Repeater {
                        model: [
                            { name: "Connection" },
                            { name: "Device" }
                        ]
                        
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            checkable: true
                            checked: index === settingsStack.currentIndex
                            
                            background: Rectangle {
                                color: parent.checked ? "#3a7fba" : (parent.hovered ? "#b3d9f2" : "transparent")
                                
                                Rectangle {
                                    visible: parent.parent.checked
                                    anchors.left: parent.left
                                    width: 4
                                    height: parent.height
                                    color: "#003c74"
                                }
                            }
                            
                            contentItem: Column {
                                spacing: 4
                                anchors.centerIn: parent
                                
                                Rectangle {
                                    width: 40
                                    height: 40
                                    radius: 4
                                    color: parent.parent.parent.checked ? "#003c74" : "#5a9fd4"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.name.substring(0, 1)
                                        color: "white"
                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }
                                
                                Text {
                                    text: modelData.name
                                    color: parent.parent.parent.checked ? "white" : "#003c74"
                                    font.pixelSize: 11
                                    horizontalAlignment: Text.AlignHCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                            
                            onClicked: {
                                console.log("Switching to settings page:", index)
                                settingsStack.currentIndex = index
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // Settings content
            StackLayout {
                id: settingsStack
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 600
                currentIndex: 0

                Item {
                    SoftwareSettingsPage {
                        anchors.fill: parent
                    }
                }
                
                Item {
                    DeviceSettingsPage {
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}