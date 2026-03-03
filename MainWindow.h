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
class TelemetryTab;

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
    TelemetryTab  *m_telemTab        = nullptr;

    // Backend logic
    ConnectionManager  m_connection;
    TelemetryParser    m_telemetryParser;
    TelemetryReceiver  m_telemetryReceiver;
    ControlSender      m_controlSender;
    ControllerState    m_lastController{};

    QTimer m_pollTimer;       // ~60 Hz: telemetry + control
    QTimer m_controllerTimer; // ~60 Hz: gamepad polling
};
