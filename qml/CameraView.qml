import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "#000000"
    
    property bool showPlaceholder: !backend.cameraConnected
    
    // Placeholder when camera is not connected
    Image {
        anchors.fill: parent
        visible: showPlaceholder
        source: "../imgs/camera_placeholder.svg" // TODO: change this
        fillMode: Image.PreserveAspectFit
    }
    
    // Video feed display
    Image {
        id: videoImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        visible: !showPlaceholder
        cache: false
        asynchronous: false
        
        // Update the image when backend provides a new frame
        Connections {
            target: backend
            function onCameraFrameChanged() {
                // Force image refresh
                videoImage.source = ""
                videoImage.source = "image://camera/frame"
            }
        }
    }
    
    // Connection status overlay
    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        width: statusText.width + 20
        height: 30
        radius: 4
        color: backend.cameraConnected ? "#aa4caf50" : "#aaf44336"
        visible: true
        
        Text {
            id: statusText
            anchors.centerIn: parent
            text: backend.cameraConnected ? "● LIVE" : "● OFFLINE"
            color: "white"
            font.pixelSize: 12
            font.bold: true
        }
    }
    
    // Camera controls
    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        spacing: 15
        visible: backend.cameraConnected
        
        // Take Picture button
        Rectangle {
            width: 60
            height: 60
            radius: 30
            color: pictureMouse.pressed ? "#1976d2" : "#2196f3"
            border.color: "white"
            border.width: 3
            
            Text {
                anchors.centerIn: parent
                text: "📷"
                font.pixelSize: 28
            }
            
            MouseArea {
                id: pictureMouse
                anchors.fill: parent
                onClicked: backend.takePicture()
            }
        }
        
        // Record button
        Rectangle {
            width: 60
            height: 60
            radius: 30
            color: backend.isRecording() ? "#d32f2f" : (recordMouse.pressed ? "#c62828" : "#f44336")
            border.color: "white"
            border.width: 3
            
            Rectangle {
                anchors.centerIn: parent
                width: backend.isRecording() ? 20 : 28
                height: backend.isRecording() ? 20 : 28
                radius: backend.isRecording() ? 2 : 14
                color: "white"
            }
            
            MouseArea {
                id: recordMouse
                anchors.fill: parent
                onClicked: {
                    if (backend.isRecording()) {
                        backend.stopRecording()
                    } else {
                        backend.startRecording()
                    }
                }
            }
        }
    }
}
