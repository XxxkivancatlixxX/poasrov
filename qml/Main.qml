import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 1600
    height: 900
    title: "PoasControll"

    Shortcut { sequence: "Ctrl+X"; onActivated: Qt.quit() }
    Shortcut { sequence: "Ctrl+Q"; onActivated: Qt.quit() }

    // ── Deep teal/aqua sky background ──────────────────────────────────────
    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0;  color: "#00d4e8" }
            GradientStop { position: 0.22; color: "#00aacf" }
            GradientStop { position: 0.52; color: "#0077aa" }
            GradientStop { position: 0.78; color: "#004a80" }
            GradientStop { position: 1.0;  color: "#002855" }
        }

        // Bokeh / aurora left glow
        Rectangle {
            anchors.fill: parent
            opacity: 1
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0;  color: "#4400cccc" }
                GradientStop { position: 0.35; color: "#00000000" }
                GradientStop { position: 0.65; color: "#00000000" }
                GradientStop { position: 1.0;  color: "#3300aadd" }
            }
        }

        // Subtle radial highlight — upper-left bloom
        Rectangle {
            x: -120; y: -80
            width: 600; height: 500
            radius: 300
            opacity: 0.18
            color: "#00eeff"
        }

        // Lower aurora pool
        Rectangle {
            x: parent.width * 0.3; y: parent.height * 0.6
            width: 700; height: 400
            radius: 350
            opacity: 0.12
            color: "#00ffcc"
        }
    }

    // ── Glossy glass toolbar ───────────────────────────────────────────────
    header: ToolBar {
        height: 52

        background: Rectangle {
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#99c8f8ff" }
                GradientStop { position: 0.5; color: "#7790d8f5" }
                GradientStop { position: 1.0; color: "#5555b8e8" }
            }

            // Top glass shine
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height * 0.5
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#66ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
            // Bottom border glow
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width; height: 1
                color: "#88ffffff"
            }
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 1
                width: parent.width; height: 1
                color: "#3300ccee"
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 10

            // ── Logo / Settings button ─────────────────────────────────────
            Button {
                id: logoButton
                Layout.preferredWidth: 190
                Layout.preferredHeight: 40

                background: Rectangle {
                    radius: 20
                    color: "transparent"
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: logoButton.down ? "#6690c8f0" : (logoButton.hovered ? "#7798d8f8" : "#6688c8f0") }
                        GradientStop { position: 0.5; color: logoButton.down ? "#4468a8d8" : (logoButton.hovered ? "#5575b8e8" : "#4460a8d8") }
                        GradientStop { position: 1.0; color: logoButton.down ? "#334888b8" : (logoButton.hovered ? "#3355a0cc" : "#2248809c") }
                    }
                    border.color: "#99ffffff"
                    border.width: 1

                    // Glass shine
                    Rectangle {
                        anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                        anchors.margins: 1; height: parent.height * 0.48; radius: 18
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#55ffffff" }
                            GradientStop { position: 1.0; color: "#00ffffff" }
                        }
                    }
                }

                contentItem: Row {
                    spacing: 8
                    anchors.centerIn: parent

                    // Aqua orb
                    Rectangle {
                        width: 30; height: 30; radius: 15
                        anchors.verticalCenter: parent.verticalCenter
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#b0eeff" }
                            GradientStop { position: 0.35; color: "#30b8f0" }
                            GradientStop { position: 0.7;  color: "#0080c8" }
                            GradientStop { position: 1.0;  color: "#004488" }
                        }
                        border.color: "#ddffffff"
                        border.width: 2
                        // Orb highlight
                        Rectangle {
                            x: 4; y: 3; width: 10; height: 8; radius: 5
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#ccffffff" }
                                GradientStop { position: 1.0; color: "#00ffffff" }
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "Q"; color: "white"
                            font.pixelSize: 14; font.bold: true
                            style: Text.Raised; styleColor: "#004488"
                        }
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 1
                        Text {
                            text: "PoasControll"
                            color: "#003a70"; font.pixelSize: 12; font.bold: true
                        }
                        Text {
                            text: "⚙ Settings"
                            color: "#0070b0"; font.pixelSize: 9
                        }
                    }
                }
                onClicked: settingsDialog.open()
            }

            Item { Layout.fillWidth: true }

            // ── Connection status pill ─────────────────────────────────────
            Rectangle {
                Layout.preferredWidth: 170
                Layout.preferredHeight: 38
                radius: 19
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: backend.mavlinkReady ? "#6690e8a0" : "#6690c870" }
                    GradientStop { position: 0.5; color: backend.mavlinkReady ? "#5555c878" : "#5555a858" }
                    GradientStop { position: 1.0; color: backend.mavlinkReady ? "#3330a858" : "#33308848" }
                }
                border.color: backend.mavlinkReady ? "#8830cc60" : "#88ffaa00"
                border.width: 1

                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                    anchors.margins: 1; height: parent.height * 0.48; radius: 17
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#55ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }

                Row {
                    anchors.centerIn: parent; spacing: 8

                    Rectangle {
                        width: 14; height: 14; radius: 7
                        anchors.verticalCenter: parent.verticalCenter
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: backend.mavlinkReady ? "#a0ffa0" : "#ffe090" }
                            GradientStop { position: 0.5; color: backend.mavlinkReady ? "#30cc50" : "#ffaa00" }
                            GradientStop { position: 1.0; color: backend.mavlinkReady ? "#0a7a28" : "#cc7000" }
                        }
                        border.color: "#ddffffff"; border.width: 1.5
                        Rectangle {
                            x: 1; y: 1; width: 5; height: 4; radius: 2.5
                            color: "#aaffffff"
                        }
                        SequentialAnimation on opacity {
                            running: !backend.mavlinkReady; loops: Animation.Infinite
                            NumberAnimation { from: 1.0; to: 0.3; duration: 700 }
                            NumberAnimation { from: 0.3; to: 1.0; duration: 700 }
                        }
                    }

                    Column {
                        spacing: 1; anchors.verticalCenter: parent.verticalCenter
                        Text {
                            text: backend.connectionStatus
                            color: backend.mavlinkReady ? "#003a18" : "#5a3000"
                            font.pixelSize: 11; font.bold: true
                        }
                        Text {
                            text: backend.mavlinkReady ? "● Ready" : "○ Waiting..."
                            color: backend.mavlinkReady ? "#10aa40" : "#cc7800"
                            font.pixelSize: 8
                        }
                    }
                }
            }
        }
    }

    FlightPage { }

    // ── Settings Dialog ────────────────────────────────────────────────────
    Dialog {
        id: settingsDialog
        modal: true
        x: (parent.width - width) / 2
        y: 40
        width: 960
        height: 680

        background: Rectangle {
            color: "transparent"
            radius: 14
            // Glass panel over the sky
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#cc00bbdd" }
                GradientStop { position: 0.5; color: "#bb0099cc" }
                GradientStop { position: 1.0; color: "#aa0077aa" }
            }
            border.color: "#88ffffff"
            border.width: 1

            // Top glass shine
            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                anchors.margins: 1; height: parent.height * 0.28; radius: 13
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }
        }

        header: Rectangle {
            height: 58; radius: 14
            color: "transparent"
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#dd00ccee" }
                GradientStop { position: 0.5; color: "#cc0099cc" }
                GradientStop { position: 1.0; color: "#aa006699" }
            }
            border.color: "#66ffffff"; border.width: 1

            Rectangle {
                anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                height: parent.height * 0.5; radius: 12
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#44ffffff" }
                    GradientStop { position: 1.0; color: "#00ffffff" }
                }
            }

            RowLayout {
                anchors.fill: parent; anchors.margins: 10; spacing: 10

                // Header orb
                Rectangle {
                    width: 36; height: 36; radius: 18
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#b0eeff" }
                        GradientStop { position: 0.4; color: "#30b8f0" }
                        GradientStop { position: 1.0; color: "#004488" }
                    }
                    border.color: "#ddffffff"; border.width: 2
                    Rectangle {
                        x: 4; y: 3; width: 12; height: 9; radius: 5
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "#bbffffff" }
                            GradientStop { position: 1.0; color: "#00ffffff" }
                        }
                    }
                    Text {
                        anchors.centerIn: parent; text: "Q"; color: "white"
                        font.pixelSize: 18; font.bold: true
                        style: Text.Raised; styleColor: "#004488"
                    }
                }

                Text {
                    text: "Settings"; color: "white"
                    font.pixelSize: 16; font.bold: true
                    style: Text.Raised; styleColor: "#004488"
                }

                Item { Layout.fillWidth: true }

                Button {
                    id: closeButton
                    width: 36; height: 36

                    background: Rectangle {
                        radius: 18
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: closeButton.down ? "#ccff4444" : (closeButton.hovered ? "#bbff5555" : "#99ff6666") }
                            GradientStop { position: 0.5; color: closeButton.down ? "#aadd2020" : (closeButton.hovered ? "#99ee3333" : "#88ee4444") }
                            GradientStop { position: 1.0; color: closeButton.down ? "#88bb1010" : (closeButton.hovered ? "#77cc2020" : "#66cc3030") }
                        }
                        border.color: "#aaffffff"; border.width: 1
                        Rectangle {
                            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                            anchors.margins: 1; height: parent.height * 0.48; radius: 16
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#55ffffff" }
                                GradientStop { position: 1.0; color: "#00ffffff" }
                            }
                        }
                    }

                    contentItem: Text {
                        text: "×"; color: "white"
                        font.pixelSize: 22; font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        style: Text.Raised; styleColor: "#991010"
                    }
                    onClicked: settingsDialog.close()
                }
            }
        }

        RowLayout {
            anchors.fill: parent; spacing: 0

            // ── Glass sidebar ──────────────────────────────────────────────
            Rectangle {
                Layout.preferredWidth: 110
                Layout.fillHeight: true
                color: "transparent"
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#aa00bbdd" }
                    GradientStop { position: 0.5; color: "#9900aabb" }
                    GradientStop { position: 1.0; color: "#880088aa" }
                }

                // Left shine
                Rectangle {
                    anchors.top: parent.top; anchors.left: parent.left; anchors.bottom: parent.bottom
                    width: parent.width * 0.5
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "#33ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                }
                // Right border
                Rectangle {
                    anchors.right: parent.right; width: 1; height: parent.height
                    color: "#55ffffff"
                }

                ColumnLayout {
                    anchors.fill: parent; anchors.topMargin: 16
                    spacing: 8

                    Repeater {
                        model: [
                            { name: "Connection", icon: "🔌" },
                            { name: "Device",     icon: "📱" },
                            { name: "Controller", icon: "🎮" }
                        ]

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 84
                            Layout.leftMargin: 6; Layout.rightMargin: 6
                            checkable: true
                            checked: index === settingsStack.currentIndex

                            background: Rectangle {
                                radius: 10
                                color: "transparent"
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: parent.parent.checked ? "#aa00ccee" : (parent.parent.hovered ? "#6600bbdd" : "#00000000") }
                                    GradientStop { position: 0.5; color: parent.parent.checked ? "#8800aacc" : (parent.parent.hovered ? "#4400aabb" : "#00000000") }
                                    GradientStop { position: 1.0; color: parent.parent.checked ? "#66007799" : (parent.parent.hovered ? "#22008899" : "#00000000") }
                                }
                                border.color: parent.parent.checked ? "#88ffffff" : (parent.parent.hovered ? "#44ffffff" : "#00ffffff")
                                border.width: 1
                                Rectangle {
                                    visible: parent.parent.checked || parent.parent.hovered
                                    anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
                                    anchors.margins: 1; height: parent.height * 0.45; radius: 8
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "#44ffffff" }
                                        GradientStop { position: 1.0; color: "#00ffffff" }
                                    }
                                }
                            }

                            contentItem: Column {
                                spacing: 5; anchors.centerIn: parent

                                Rectangle {
                                    width: 44; height: 44; radius: 22
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "#b0eeff" }
                                        GradientStop { position: 0.4; color: "#30b8f0" }
                                        GradientStop { position: 1.0; color: "#004488" }
                                    }
                                    border.color: "#ddffffff"; border.width: 2
                                    Rectangle {
                                        x: 4; y: 3; width: 14; height: 11; radius: 7
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: "#aaffffff" }
                                            GradientStop { position: 1.0; color: "#00ffffff" }
                                        }
                                    }
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.icon; font.pixelSize: 22
                                    }
                                }

                                Text {
                                    text: modelData.name
                                    color: "white"
                                    font.pixelSize: 10; font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    style: Text.Raised; styleColor: "#004488"
                                }
                            }

                            onClicked: settingsStack.currentIndex = index
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // ── Settings content ───────────────────────────────────────────
            StackLayout {
                id: settingsStack
                Layout.fillWidth: true; Layout.fillHeight: true
                currentIndex: 0

                Item { SoftwareSettingsPage { anchors.fill: parent } }
                Item { DeviceSettingsPage   { anchors.fill: parent } }
                Item { ControllerSettingsPage { anchors.fill: parent } }
            }
        }
    }
}
