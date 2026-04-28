#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QtGlobal>

#include "connection.h"
#include "telemetry_parser.h"
#include "control_sender.h"
#include "ROV.h"
#include "joystick_control.h"
#include "video_provider.h"

struct TelemetrySnapshot {
    float batteryVoltage = 0.0f;
    float batteryCurrent = 0.0f;
    int   batteryPercentage = 0;
    float depth = 0.0f;
    float temperature = 0.0f;
    float pressure = 0.0f;
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    bool  armed = false;
};

class Backend : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool mavlinkReady READ mavlinkReady NOTIFY mavlinkReadyChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY armedChanged)
    Q_PROPERTY(bool armingInProgress READ armingInProgress NOTIFY armingInProgressChanged)
    Q_PROPERTY(int motorTestCooldownMs READ motorTestCooldownMs NOTIFY motorTestCooldownChanged)
    Q_PROPERTY(bool motorTestCoolingDown READ motorTestCoolingDown NOTIFY motorTestCooldownChanged)
    Q_PROPERTY(float batteryVoltage READ batteryVoltage NOTIFY telemetryChanged)
    Q_PROPERTY(int batteryPercent READ batteryPercent NOTIFY telemetryChanged)
    Q_PROPERTY(float depth READ depth NOTIFY telemetryChanged)
    Q_PROPERTY(float roll READ roll NOTIFY telemetryChanged)
    Q_PROPERTY(float pitch READ pitch NOTIFY telemetryChanged)
    Q_PROPERTY(float yaw READ yaw NOTIFY telemetryChanged)
    Q_PROPERTY(QString cameraUrl READ cameraUrl NOTIFY cameraUrlChanged)
    Q_PROPERTY(bool cameraConnected READ cameraConnected NOTIFY cameraConnectedChanged)
    Q_PROPERTY(QImage cameraFrame READ cameraFrame NOTIFY cameraFrameChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    QString connectionStatus() const { return m_connectionStatus; }
    bool mavlinkReady() const { return m_mavlinkReady; }
    bool armed() const { return m_telemetry.armed; }
    bool armingInProgress() const { return m_armingInProgress; }
    int motorTestCooldownMs() const;
    bool motorTestCoolingDown() const { return motorTestCooldownMs() > 0; }

    float batteryVoltage() const { return m_telemetry.batteryVoltage; }
    int   batteryPercent() const { return m_telemetry.batteryPercentage; }
    float depth() const { return m_telemetry.depth; }
    float roll() const { return m_telemetry.roll; }
    float pitch() const { return m_telemetry.pitch; }
    float yaw() const { return m_telemetry.yaw; }
    QString cameraUrl() const { return m_cameraUrl; }
    bool cameraConnected() const { return m_videoProvider.isConnected(); }
    QImage cameraFrame() const { return m_videoProvider.currentFrame(); }

public slots:
    // Connection control (MAVLink-only for now)
    Q_INVOKABLE void connectTcp(const QString &host, int port);
    Q_INVOKABLE void connectUdp(const QString &host, int port);
    Q_INVOKABLE void disconnectLink();

    // High-level actions (MAVLink only)
    Q_INVOKABLE void armVehicle();
    Q_INVOKABLE void disarmVehicle();
    Q_INVOKABLE void forceArmVehicle();  // Bypass pre-arm checks
    Q_INVOKABLE void setMotorTest(int motorIndex, qreal throttle); // 0..7, 0..1
    Q_INVOKABLE void setAllThrottle(qreal throttle);               // 0..1
    
    // Joystick control
    Q_INVOKABLE void setJoystickEnabled(bool enabled);
    Q_INVOKABLE void setJoystickMaxThrottle(qreal max);            // 0..1 safety limit
    Q_INVOKABLE bool isJoystickConnected() const;
    
    // Camera control
    Q_INVOKABLE void setCameraUrl(const QString &url);
    Q_INVOKABLE void connectCamera();
    Q_INVOKABLE void disconnectCamera();

signals:
    void connectionStatusChanged();
    void mavlinkReadyChanged();
    void armedChanged();
    void armingInProgressChanged();
    void motorTestCooldownChanged();
    void telemetryChanged();
    void logMessage(const QString &msg);
    void cameraUrlChanged();
    void cameraConnectedChanged();
    void cameraFrameChanged();

private slots:
    void pollTelemetry();
    void updateJoystick();
    void sendRCHeartbeat();
    void updateCooldownDisplay();

private:
    void updateConnectionStatus();
    void ensureRov();
    void handleMavlinkHeartbeatState();

    ConnectionManager m_connection;
    TelemetryParser   m_telemetryParser;
    ControlSender     m_controlSender;
    ROV              *m_rov = nullptr;
    JoystickControl   m_joystick;
    VideoProvider     m_videoProvider;

    QTimer m_pollTimer;
    QTimer m_joystickTimer;
    QTimer m_rcHeartbeatTimer;  // Send RC heartbeat to prevent failsafe
    QTimer m_cooldownUpdateTimer;  // Update cooldown display

    QString m_connectionStatus;
    bool    m_mavlinkReady = false;
    bool    m_armingInProgress = false;
    bool    m_motorTestActive = false;
    int     m_activeMotorIndex = 0;
    qreal   m_activeMotorThrottle = 0.0;
    qint64  m_lastForceArmAttemptMs = 0;
    qint64  m_nextMotorTestAllowedMs = 0;
    TelemetrySnapshot m_telemetry;
    
    QString m_cameraUrl = "rtsp://192.168.1.2:8554/cam";
};

