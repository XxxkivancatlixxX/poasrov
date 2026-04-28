import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "#000000"
    
    property bool showPlaceholder: !backend.cameraConnected
    
    // Placeholder when camera is not connected
    Rectangle {
        anchors.fill: parent
        color: "#1a1a1a"
        visible: showPlaceholder
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "📹"
                font.pixelSize: 64
                color: "#666666"
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Camera Disconnected"
                color: "#999999"
                font.pixelSize: 18
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Go to Software Settings to connect"
                color: "#666666"
                font.pixelSize: 12
            }
        }
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
}
