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
                text: "Controller Configuration"
                color: "#003c74"
                font.pixelSize: 22
                font.bold: true
            }

            // Profile Info
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
                    width: profileLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: profileLabel
                        text: "Profile Settings"
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    // Info box
                    Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        color: "#e3f2fd"
                        border.color: "#2196f3"
                        border.width: 1
                        radius: 4

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10

                            Rectangle {
                                width: 40
                                height: 40
                                radius: 20
                                color: "#2196f3"
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "i"
                                    color: "white"
                                    font.pixelSize: 24
                                    font.bold: true
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    text: "Choose Your Control Mode:"
                                    color: "#1565c0"
                                    font.pixelSize: 12
                                    font.bold: true
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: "• Simple Mode: Just fly! Controller works out of the box.\n• QGC Default: Standard 8-motor configuration for advanced users.\n• Custom: Configure individual motors below."
                                    color: "#0d47a1"
                                    font.pixelSize: 10
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Text {
                            text: "Active Profile:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            text: backend.getControllerProfileName()
                            color: "#2e7d32"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        
                        Button {
                            text: "Simple Mode"
                            Layout.fillWidth: true
                            
                            background: Rectangle {
                                radius: 4
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: parent.parent.down ? "#66bb6a" : (parent.parent.hovered ? "#81c784" : "#4caf50") }
                                    GradientStop { position: 1.0; color: parent.parent.down ? "#43a047" : (parent.parent.hovered ? "#66bb6a" : "#388e3c") }
                                }
                                border.color: "#2e7d32"
                                border.width: 2
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                backend.loadSimpleModeProfile()
                                motorSelector.currentIndex = 0
                            }
                            
                            ToolTip.visible: hovered
                            ToolTip.text: "Easy mode: Just fly! No motor configuration needed."
                        }
                        
                        Button {
                            text: "QGC Default"
                            Layout.fillWidth: true
                            
                            background: Rectangle {
                                radius: 4
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: parent.parent.down ? "#5a9fd4" : (parent.parent.hovered ? "#7eb4ea" : "#3a7fba") }
                                    GradientStop { position: 1.0; color: parent.parent.down ? "#3a7fba" : (parent.parent.hovered ? "#5a9fd4" : "#306ba3") }
                                }
                                border.color: "#003c74"
                                border.width: 2
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                backend.resetToDefaultProfile()
                                motorSelector.currentIndex = 0
                            }
                            
                            ToolTip.visible: hovered
                            ToolTip.text: "Standard QGroundControl 8-motor configuration"
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Text {
                            text: "Number of Motors:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        SpinBox {
                            id: numMotorsSpinBox
                            from: 1
                            to: 8
                            value: backend.getNumMotors()
                            onValueModified: backend.setNumMotors(value)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Text {
                            text: "Deadzone:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Slider {
                            id: deadzoneSlider
                            Layout.fillWidth: true
                            from: 0.0
                            to: 0.3
                            value: 0.1
                            stepSize: 0.01
                            onMoved: backend.setJoystickDeadzone(value)
                        }

                        Text {
                            text: Math.round(deadzoneSlider.value * 100) + "%"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                            Layout.preferredWidth: 50
                        }
                    }
                }
            }

            // Motor Mapping Configuration
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
                    width: motorMappingLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: motorMappingLabel
                        text: "Motor Mapping"
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
                        Layout.fillWidth: true
                        text: "Configure which controller inputs control each motor. Multiple inputs can be combined per motor."
                        color: "#003c74"
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12
                        
                        Text {
                            text: "Select Motor:"
                            color: "#003c74"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        ComboBox {
                            id: motorSelector
                            model: ["Motor 1", "Motor 2", "Motor 3", "Motor 4", 
                                    "Motor 5", "Motor 6", "Motor 7", "Motor 8"]
                            currentIndex: 0
                            Layout.preferredWidth: 140
                            onCurrentIndexChanged: updateMappingDisplay()
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        color: "#f5f5f5"
                        border.color: "#cccccc"
                        border.width: 1
                        radius: 4

                        ScrollView {
                            anchors.fill: parent
                            anchors.margins: 8
                            clip: true

                            ColumnLayout {
                                id: mappingDisplay
                                width: parent.width
                                spacing: 4

                                Text {
                                    text: "Current mappings for Motor " + (motorSelector.currentIndex + 1) + ":"
                                    color: "#003c74"
                                    font.pixelSize: 11
                                    font.bold: true
                                }

                                Repeater {
                                    id: mappingRepeater
                                    model: ListModel { id: mappingModel }
                                    
                                    delegate: Text {
                                        text: model.description
                                        color: "#555555"
                                        font.pixelSize: 10
                                    }
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Button {
                            text: "Clear Mappings"
                            onClicked: {
                                backend.clearMotorMappings(motorSelector.currentIndex)
                                updateMappingDisplay()
                            }
                        }

                        Button {
                            text: "Add Custom Mapping"
                            onClicked: customMappingDialog.open()
                        }
                    }
                }
            }

            // Quick Presets
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
                    width: presetsLabel.width + 20
                    height: 30
                    radius: 4
                    
                    Text {
                        id: presetsLabel
                        text: "Control Guide"
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
                        text: "Default Controls (Simple & QGC Mode):"
                        color: "#003c74"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Grid {
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 8
                        Layout.fillWidth: true

                        Text {
                            text: "Left Stick:"
                            color: "#003c74"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Text {
                            text: "Forward/Back + Strafe Left/Right"
                            color: "#555555"
                            font.pixelSize: 11
                        }

                        Text {
                            text: "Right Stick X:"
                            color: "#003c74"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Text {
                            text: "Rotate (Yaw)"
                            color: "#555555"
                            font.pixelSize: 11
                        }

                        Text {
                            text: "Right Trigger:"
                            color: "#003c74"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Text {
                            text: "Ascend (Up)"
                            color: "#555555"
                            font.pixelSize: 11
                        }

                        Text {
                            text: "Left Trigger:"
                            color: "#003c74"
                            font.pixelSize: 11
                            font.bold: true
                        }
                        Text {
                            text: "Descend (Down)"
                            color: "#555555"
                            font.pixelSize: 11
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "#cccccc"
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "Custom Motor Configuration:\n" +
                              "To make left joystick control specific motors (e.g., motors 1, 2, 3):\n" +
                              "1. Select each motor from the dropdown\n" +
                              "2. Click 'Add Custom Mapping'\n" +
                              "3. Choose input (e.g., 'Left Stick Y')\n" +
                              "4. Set scale (usually 1.0) and invert if needed\n" +
                              "5. Click 'Add' and repeat for other motors"
                        color: "#003c74"
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    // Custom Mapping Dialog
    Dialog {
        id: customMappingDialog
        title: "Add Motor Mapping"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            spacing: 12
            anchors.fill: parent

            Text {
                text: "Motor: " + (motorSelector.currentIndex + 1)
                font.bold: true
            }

            RowLayout {
                Text { text: "Input Source:" }
                ComboBox {
                    id: inputSourceCombo
                    model: ["Left Stick X", "Left Stick Y", "Right Stick X", "Right Stick Y", 
                            "Left Trigger", "Right Trigger"]
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Text { text: "Scale:" }
                SpinBox {
                    id: scaleSpinBox
                    from: -100
                    to: 100
                    value: 100
                    stepSize: 10
                    
                    property real realValue: value / 100.0
                    
                    textFromValue: function(value) {
                        return (value / 100.0).toFixed(2)
                    }
                }
            }

            CheckBox {
                id: invertCheckBox
                text: "Invert Input"
            }

            RowLayout {
                Button {
                    text: "Add"
                    onClicked: {
                        backend.addMotorMapping(
                            motorSelector.currentIndex,
                            0, // INPUT_AXIS
                            inputSourceCombo.currentIndex,
                            scaleSpinBox.realValue,
                            invertCheckBox.checked
                        )
                        updateMappingDisplay()
                        customMappingDialog.close()
                    }
                }
                Button {
                    text: "Cancel"
                    onClicked: customMappingDialog.close()
                }
            }
        }
    }

    function updateMappingDisplay() {
        mappingModel.clear()
        
        var mappings = backend.getMotorMappings(motorSelector.currentIndex)
        
        if (mappings.length === 0) {
            mappingModel.append({description: "No mappings configured"})
            return
        }
        
        var inputNames = ["Left Stick X", "Left Stick Y", "Right Stick X", "Right Stick Y", 
                          "Left Trigger", "Right Trigger"]
        
        for (var i = 0; i < mappings.length; i++) {
            var m = mappings[i]
            var inputName = m.inputId < inputNames.length ? inputNames[m.inputId] : "Unknown"
            var desc = inputName + " × " + m.scale.toFixed(2)
            if (m.inverted) desc += " (inverted)"
            mappingModel.append({description: desc})
        }
    }

    Component.onCompleted: {
        updateMappingDisplay()
    }
}
