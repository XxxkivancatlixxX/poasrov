// MainWindow.h
#pragma once

#include <QMainWindow>
#include <QTimer>

#include "input.h"
#include "connection.h"
#include "telemetry_parser.h"
#include "telemetry_receiver.h"
#include "control_sender.h"

class QTabWidget;
class QLabel;
class ConnectionTab;
class VideoWidget;
class VideoDecoder;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // From ConnectionTab
    void onConnectionRequestTCP(const QString &host, quint16 port);
    void onDisconnectRequested();

    // Periodic timers
    void onPeriodicUpdate();   // telemetry + control
    void onControllerPolled(); // SDL controller polling

private:
    void setupUi();
    void setupTimers();
    void updateConnectionStatusLabel();
    void processTelemetry();
    void sendControlPacket();

    // UI
    QTabWidget    *m_tabs            = nullptr;
    QLabel        *m_controllerLabel = nullptr;
    QLabel        *m_connectionLabel = nullptr;
    ConnectionTab *m_connectionTab   = nullptr;
    VideoWidget   *m_videoWidget     = nullptr;
    VideoDecoder  *m_videoDecoder    = nullptr;

    // Backend logic
    ConnectionManager  m_connection;        // TCP/UDP/serial manager [3]
    TelemetryParser    m_telemetryParser;   // parse TelemetryPacket [18]
    TelemetryReceiver  m_telemetryReceiver; // store latest state [20][21]
    ControlSender      m_controlSender;     // build ControlPacket [4][5]
    ControllerState    m_lastController{};  // from input.* [9][10]

    QTimer m_pollTimer;       // ~60 Hz: telemetry + control
    QTimer m_controllerTimer; // ~60 Hz: gamepad polling
};
