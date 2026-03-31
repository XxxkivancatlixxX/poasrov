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
                text: "Vehicle Configuration"
                color: "#003c74"
                font.pixelSize: 22
                font.bold: true
            }

            // Joystick Configuration
            GroupBox {
                Layout.fillWidth: true
                
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
                    width: joystickLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: joystickLabel
                        text: "Joystick Configuration"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }


                RowLayout {
                    anchors.fill: parent
                    spacing: 12
                    
                    Text {
                        text: "Max Throttle:"
                        color: "#003c74"
                        font.pixelSize: 12
                    }
                    
                    Slider {
                        id: maxThrottleSlider
                        Layout.fillWidth: true
                        from: 0.1
                        to: 1.0
                        value: 0.5
                        stepSize: 0.05
                        onMoved: backend.setJoystickMaxThrottle(value)
                    }

                    Text {
                        text: Math.round(maxThrottleSlider.value * 100) + "%"
                        color: "#003c74"
                        font.pixelSize: 12
                        font.bold: true
                        Layout.preferredWidth: 50
                    }
                }
            }

            // Motor Testing
            GroupBox {
                Layout.fillWidth: true
                
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
                    width: motorLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: motorLabel
                        text: "Motor Testing"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }


                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    rowSpacing: 10
                    columnSpacing: 16

                    Repeater {
                        model: 8
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            
                            Text {
                                text: "Motor " + (index + 1) + ":"
                                color: "#003c74"
                                font.pixelSize: 12
                                Layout.preferredWidth: 60
                            }

                            Slider {
                                id: motorSlider
                                Layout.fillWidth: true
                                from: 0.0
                                to: 1.0
                                value: 0.0
                                stepSize: 0.01
                                enabled: backend.mavlinkReady && backend.armed
                                
                                property int motorIndex: index
                                
                                onMoved: {
                                    backend.setMotorTest(motorIndex, value)
                                }
                                
                                onPressedChanged: {
                                    if (!pressed) {
                                        value = 0.0
                                        backend.setMotorTest(motorIndex, 0.0)
                                    }
                                }
                            }

                            Text {
                                text: Math.round(motorSlider.value * 100) + "%"
                                color: motorSlider.enabled ? "#003c74" : "#999999"
                                font.pixelSize: 11
                                font.bold: true
                                Layout.preferredWidth: 45
                            }
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
