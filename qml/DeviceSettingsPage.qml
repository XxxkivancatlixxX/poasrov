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


                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Text {
                            text: "Joystick Control:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Switch {
                            id: joystickEnableSwitch
                            checked: true
                            onToggled: backend.setJoystickEnabled(checked)
                            Component.onCompleted: backend.setJoystickEnabled(true)
                        }
                        
                        Text {
                            text: joystickEnableSwitch.checked ? "ENABLED" : "DISABLED"
                            color: joystickEnableSwitch.checked ? "#2e7d32" : "#c62828"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Text {
                            text: backend.isJoystickConnected() ? "Controller Connected" : "No Controller"
                            color: backend.isJoystickConnected() ? "#2e7d32" : "#e65100"
                            font.pixelSize: 11
                        }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
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
            }

            // Motor Testing
            GroupBox {
                id: motorTestBox
                Layout.fillWidth: true
                property bool motorTestMode: false
                property int selectedMotorIndex: 0
                property int previousMotorIndex: 0
                
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


                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10
                    property bool updatingAllFromRelease: false

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: "Motor Test Enable"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }

                        Switch {
                            id: motorTestArmSwitch
                            enabled: backend.mavlinkReady && !backend.armingInProgress
                            checked: motorTestBox.motorTestMode

                            onToggled: {
                                motorTestBox.motorTestMode = checked
                                if (!checked) {
                                    backend.setMotorTest(0, 0.0)
                                    backend.setAllThrottle(0.0)
                                }
                            }
                        }

                        Text {
                            text: motorTestBox.motorTestMode ? "TEST MODE ON" : "TEST MODE OFF"
                            color: motorTestBox.motorTestMode ? "#c62828" : "#2e7d32"
                            font.pixelSize: 11
                            font.bold: true
                            Layout.preferredWidth: 120
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        visible: motorTestBox.motorTestMode
                        text: backend.motorTestCoolingDown
                              ? "Individual motor cooldown: " + (backend.motorTestCooldownMs / 1000.0).toFixed(1) + "s"
                              : (!backend.armed
                                 ? "Individual motor test requires the vehicle to be armed."
                                 : "Individual motor test ready.")
                        color: backend.motorTestCoolingDown ? "#c62828" : "#003c74"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: "All Motors:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                            Layout.preferredWidth: 70
                        }

                        Slider {
                            id: allMotorsSlider
                            Layout.fillWidth: true
                            from: 0.0
                            to: 1.0
                            value: 0.0
                            stepSize: 0.01
                            enabled: backend.mavlinkReady && motorTestBox.motorTestMode && !backend.armingInProgress && !backend.motorTestCoolingDown

                            onValueChanged: {
                                if (!motorTestBox.updatingAllFromRelease && enabled && pressed) {
                                    backend.setAllThrottle(value)
                                }
                            }

                            onPressedChanged: {
                                if (!pressed) {
                                    motorTestBox.updatingAllFromRelease = true
                                    value = 0.0
                                    motorTestBox.updatingAllFromRelease = false
                                    backend.setAllThrottle(0.0)
                                }
                            }
                        }

                        Text {
                            text: Math.round(allMotorsSlider.value * 100) + "%"
                            color: allMotorsSlider.enabled ? "#003c74" : "#999999"
                            font.pixelSize: 11
                            font.bold: true
                            Layout.preferredWidth: 45
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Text {
                            text: "Selected Motor:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }

                        ComboBox {
                            id: motorSelector
                            enabled: backend.mavlinkReady && motorTestBox.motorTestMode && !backend.armingInProgress
                            model: ["Motor 1", "Motor 2", "Motor 3", "Motor 4", "Motor 5", "Motor 6", "Motor 7", "Motor 8"]
                            currentIndex: motorTestBox.selectedMotorIndex
                            Layout.preferredWidth: 140

                            onCurrentIndexChanged: {
                                if (motorTestBox.selectedMotorIndex === currentIndex) {
                                    return
                                }
                                // Stop previously selected motor immediately before switching.
                                backend.setMotorTest(motorTestBox.selectedMotorIndex, 0.0)
                                motorTestBox.previousMotorIndex = motorTestBox.selectedMotorIndex
                                motorTestBox.selectedMotorIndex = currentIndex
                                singleMotorSlider.value = 0.0
                            }
                        }

                        Text {
                            text: "Individual Motor:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                            Layout.preferredWidth: 110
                        }

                        Slider {
                            id: singleMotorSlider
                            Layout.fillWidth: true
                            from: 0.0
                            to: 1.0
                            value: 0.0
                            stepSize: 0.01
                            enabled: backend.mavlinkReady && motorTestBox.motorTestMode && !backend.armingInProgress

                            property bool updatingFromRelease: false

                            onValueChanged: {
                                // Deadman behavior: only command thrust while actively pressing slider.
                                if (!updatingFromRelease && enabled && pressed) {
                                    backend.setMotorTest(motorTestBox.selectedMotorIndex, value)
                                }
                            }

                            onPressedChanged: {
                                if (!pressed) {
                                    updatingFromRelease = true
                                    value = 0.0
                                    updatingFromRelease = false
                                    backend.setMotorTest(motorTestBox.selectedMotorIndex, 0.0)
                                }
                            }
                        }

                        Text {
                            text: Math.round(singleMotorSlider.value * 100) + "%"
                            color: singleMotorSlider.enabled ? "#003c74" : "#999999"
                            font.pixelSize: 11
                            font.bold: true
                            Layout.preferredWidth: 45
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
