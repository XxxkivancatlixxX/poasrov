// backend for za poasrov qground copy jahoy jahoyyyy

#include "Backend.h"
#include "input.h"

#include <QDebug>
#include <QDateTime>
#include <QString>
#include <cstdio>

namespace {
void appendDebugLog(const char *hypothesisId,
                    const char *location,
                    const char *message,
                    const QString &data,
                    const char *runId = "pre-fix")
{
    FILE *f = std::fopen("/home/vujuvuju/rov/PCside/.cursor/debug.log", "a");
    if (!f) return;
    const qint64 ts = QDateTime::currentMSecsSinceEpoch();
    std::fprintf(
        f,
        "{\"id\":\"log_%lld_%s\",\"timestamp\":%lld,\"location\":\"%s\",\"message\":\"%s\","
        "\"data\":{%s},\"runId\":\"%s\",\"hypothesisId\":\"%s\"}\n",
        static_cast<long long>(ts), hypothesisId, static_cast<long long>(ts), location, message,
        data.toUtf8().constData(), runId, hypothesisId);
    std::fclose(f);
}
}

Backend::Backend(QObject *parent)
    : QObject(parent)
{
    // Initialize SDL for joystick support
    input_init();
    
    // Connect video provider signals
    connect(&m_videoProvider, &VideoProvider::frameChanged, 
            this, &Backend::cameraFrameChanged);
    connect(&m_videoProvider, &VideoProvider::connectionChanged, 
            this, &Backend::cameraConnectedChanged);
    connect(&m_videoProvider, &VideoProvider::errorOccurred,
            this, &Backend::logMessage);
    
    m_pollTimer.setInterval(30); // ~33 Hz
    connect(&m_pollTimer, &QTimer::timeout, this, &Backend::pollTelemetry);
    m_pollTimer.start();
    
    m_joystickTimer.setInterval(50); // 20 Hz for joystick updates
    connect(&m_joystickTimer, &QTimer::timeout, this, &Backend::updateJoystick);
    m_joystickTimer.start();
    
    // Send heartbeat and motor-test refresh at 10Hz.
    // ArduSub motor-test watchdog is 500ms; 10Hz gives margin without command spam.
    m_rcHeartbeatTimer.setInterval(100);
    connect(&m_rcHeartbeatTimer, &QTimer::timeout, this, &Backend::sendRCHeartbeat);
    m_rcHeartbeatTimer.start();
    
    // Update cooldown display at 10Hz for smooth countdown
    m_cooldownUpdateTimer.setInterval(100);
    connect(&m_cooldownUpdateTimer, &QTimer::timeout, this, &Backend::updateCooldownDisplay);
    m_cooldownUpdateTimer.start();

    updateConnectionStatus();
}

int Backend::motorTestCooldownMs() const
{
    const qint64 remaining = m_nextMotorTestAllowedMs - QDateTime::currentMSecsSinceEpoch();
    return remaining > 0 ? static_cast<int>(remaining) : 0;
}

void Backend::connectTcp(const QString &host, int port)
{
    disconnectLink();
    m_connection.create_tcp_connection(host.toStdString(), static_cast<uint16_t>(port));
    if (!m_connection.connect()) {
        emit logMessage(QStringLiteral("TCP connect failed: %1")
                        .arg(QString::fromStdString(m_connection.get_error())));
        updateConnectionStatus();
        return;
    }
    emit logMessage(QStringLiteral("TCP link opened to %1:%2").arg(host).arg(port));
    m_telemetryParser.reset();
    m_mavlinkReady = false;
    emit mavlinkReadyChanged();
    updateConnectionStatus();
}

void Backend::connectUdp(const QString &host, int port)
{
    disconnectLink();
    m_connection.create_udp_connection(host.toStdString(), static_cast<uint16_t>(port));
    if (!m_connection.connect()) {
        emit logMessage(QStringLiteral("UDP open failed: %1")
                        .arg(QString::fromStdString(m_connection.get_error())));
        updateConnectionStatus();
        return;
    }
    emit logMessage(QStringLiteral("UDP port open to %1:%2 (waiting for MAVLink heartbeat)")
                    .arg(host).arg(port));
    m_telemetryParser.reset();
    m_mavlinkReady = false;
    emit mavlinkReadyChanged();
    updateConnectionStatus();
}

void Backend::disconnectLink()
{
    if (m_rov) {
        delete m_rov;
        m_rov = nullptr;
    }
    m_connection.disconnect();
    m_telemetryParser.reset();
    m_mavlinkReady = false;
    m_motorTestActive = false;
    m_nextMotorTestAllowedMs = 0;
    emit motorTestCooldownChanged();
    emit mavlinkReadyChanged();
    updateConnectionStatus();
}

void Backend::armVehicle()
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;
    if (m_armingInProgress) return;  // Prevent multiple commands

    qDebug() << "ARM command sent";
    m_armingInProgress = true;
    emit armingInProgressChanged();

    ensureRov();
    if (!m_rov) {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        return;
    }

    m_rov->arm();
    
    // Reset arming flag after a delay
    QTimer::singleShot(3000, this, [this]() {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        qDebug() << "Arming timeout cleared";
    });
}

void Backend::forceArmVehicle()
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;
    if (m_armingInProgress) return;

    qDebug() << "FORCE ARM command sent (bypassing pre-arm checks)";
    emit logMessage("Force arming vehicle (bypassing safety checks)");
    m_armingInProgress = true;
    emit armingInProgressChanged();

    ensureRov();
    if (!m_rov) {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        return;
    }

    m_rov->forceArm();
    
    QTimer::singleShot(3000, this, [this]() {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        qDebug() << "Force arming timeout cleared";
    });
}

void Backend::disarmVehicle()
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;
    if (m_armingInProgress) return;  // Prevent multiple commands

    qDebug() << "DISARM command sent";
    m_armingInProgress = true;
    emit armingInProgressChanged();

    ensureRov();
    if (!m_rov) {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        return;
    }

    m_rov->disarm();
    
    // Reset arming flag after a delay
    QTimer::singleShot(3000, this, [this]() {
        m_armingInProgress = false;
        emit armingInProgressChanged();
        qDebug() << "Disarming timeout cleared";
    });
}
// SOOO SOMETHİNG HAPPENİNG IDK WHAT
void Backend::setMotorTest(int motorIndex, qreal throttle)
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;
    if (motorIndex < 0 || motorIndex > 7) return;

    if (throttle < 0.0) throttle = 0.0;
    if (throttle > 1.0) throttle = 1.0;

    // Avoid accidental "touch" values immediately starting a full motor test cycle.
    constexpr qreal kMotorTestMinThrottle = 0.08;
    if (throttle < kMotorTestMinThrottle) {
        throttle = 0.0;
    }

    ensureRov();
    if (!m_rov) return;

    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();

    if (throttle > 0.0 && nowMs < m_nextMotorTestAllowedMs) {
        emit motorTestCooldownChanged();
        return;
    }

    m_activeMotorIndex = motorIndex;
    m_activeMotorThrottle = throttle;
    // Individual motor testing uses DO_MOTOR_TEST and must be explicitly armed by the user.
    if (!m_telemetry.armed) {
        if (throttle > 0.0) {
            emit logMessage("Individual motor test requires the vehicle to be armed first.");
        }
        m_motorTestActive = false;
        return;
    }

    if (throttle <= 0.0) {
        // Hard stop immediately when user releases slider.
        m_motorTestActive = false;
        m_activeMotorThrottle = 0.0f;
        // Don't send 0% command - just stop refreshing
        return;
    }

    // Send immediately so slider response is deterministic and low-latency.
    // Continuous refreshes are handled in sendRCHeartbeat() while active.
    m_motorTestActive = true;
    m_rov->setMotorThrottle(static_cast<uint8_t>(motorIndex),
                            static_cast<float>(throttle));
}

void Backend::setAllThrottle(qreal throttle)
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;

    if (throttle < 0.0) throttle = 0.0;
    if (throttle > 1.0) throttle = 1.0;

    ensureRov();
    if (!m_rov) return;

    // Switching to "all motors" mode should cancel single-motor test refresh.
    m_motorTestActive = false;
    m_activeMotorThrottle = 0.0;

    m_rov->setAllMotorThrottle(static_cast<float>(throttle));
}

void Backend::pollTelemetry()
{
    if (!m_connection.is_connected())
        return;

    uint8_t buffer[2048];
    uint16_t len = 0;
    if (!m_connection.receive(buffer, sizeof(buffer), len) || len == 0)
        return;

    // Feed TelemetryParser (which internally uses MAVLinkParser when needed)
    for (uint16_t i = 0; i < len; ++i) {
        if (m_telemetryParser.parse_byte(buffer[i])) {
            std::string statusTextStd;
            uint8_t statusSeverity = 0;
            if (m_telemetryParser.takeLatestStatusText(statusTextStd, statusSeverity)) {
                const QString statusText = QString::fromStdString(statusTextStd);
                emit logMessage(QStringLiteral("AP: %1").arg(statusText));

                if (statusText.contains(QStringLiteral("10 second cooldown required after motor test"),
                                        Qt::CaseInsensitive) ||
                    statusText.contains(QStringLiteral("motor test initialization failed"),
                                        Qt::CaseInsensitive) ||
                    statusText.contains(QStringLiteral("Motor test timed out!"),
                                        Qt::CaseInsensitive)) {
                    m_nextMotorTestAllowedMs = QDateTime::currentMSecsSinceEpoch() + 10000;
                    emit motorTestCooldownChanged();
                }
            }

            uint16_t ackCommand = 0;
            uint8_t ackResult = 0;
            if (m_telemetryParser.takeLatestCommandAck(ackCommand, ackResult)) {
                if (ackCommand == MAV_CMD_DO_MOTOR_TEST && ackResult != MAV_RESULT_ACCEPTED) {
                    emit logMessage(QStringLiteral("DO_MOTOR_TEST rejected (result=%1)").arg(ackResult));
                    m_nextMotorTestAllowedMs = QDateTime::currentMSecsSinceEpoch() + 10000;
                    emit motorTestCooldownChanged();
                }
            }

            RobotState rs;
            if (m_telemetryParser.get_packet(rs)) {
                bool wasArmed = m_telemetry.armed;
                
                m_telemetry.batteryVoltage    = rs.battery.voltage;
                m_telemetry.batteryCurrent    = rs.battery.current;
                m_telemetry.batteryPercentage = rs.battery.percentage;
                m_telemetry.depth             = rs.sensors.depth;
                m_telemetry.temperature       = rs.sensors.temperature;
                m_telemetry.pressure          = rs.sensors.pressure;
                m_telemetry.roll              = rs.roll;
                m_telemetry.pitch             = rs.pitch;
                m_telemetry.yaw               = rs.yaw;
                m_telemetry.armed             = rs.armed != 0;

                emit telemetryChanged();
                
                if (wasArmed != m_telemetry.armed) {
                    qDebug() << "Armed state changed:" << wasArmed << "->" << m_telemetry.armed;
                    emit armedChanged();

                    if (m_telemetry.armed && m_armingInProgress) {
                        m_armingInProgress = false;
                        emit armingInProgressChanged();
                    }

                    if (!m_telemetry.armed) {
                        m_motorTestActive = false;
                        m_activeMotorThrottle = 0.0;
                    }
                }

                handleMavlinkHeartbeatState();
            }
        }
    }
}

void Backend::handleMavlinkHeartbeatState()
{
    bool had = m_mavlinkReady;
    if (m_telemetryParser.has_mavlink_heartbeat()) {
        m_mavlinkReady = true;
    }
    if (m_mavlinkReady != had)
        emit mavlinkReadyChanged();
    updateConnectionStatus();
}

void Backend::ensureRov()
{
    if (m_rov || !m_connection.is_connected())
        return;

    // Prefer TCP; fall back to UDP
    if (TCPConnection *tcp = m_connection.get_tcp_connection()) {
        int sock = tcp->get_socket();
        sockaddr_in addr = tcp->get_target_addr();
        m_rov = new ROV(sock, addr);
        emit logMessage("ROV MAVLink controller initialized (TCP)");
    } else if (UDPConnection *udp = m_connection.get_udp_connection()) {
        int sock = udp->get_socket();
        sockaddr_in addr = udp->get_target_addr();
        m_rov = new ROV(sock, addr);
        emit logMessage("ROV MAVLink controller initialized (UDP)");
    }
}

void Backend::updateConnectionStatus()
{
    if (!m_connection.is_connected()) {
        m_connectionStatus = QStringLiteral("Disconnected");
    } else if (!m_mavlinkReady) {
        m_connectionStatus = QStringLiteral("Link open – waiting for MAVLink heartbeat");
    } else {
        m_connectionStatus = QStringLiteral("Connected (MAVLink ready)");
    }
    emit connectionStatusChanged();
}


void Backend::setJoystickEnabled(bool enabled)
{
    m_joystick.set_enabled(enabled);
    emit logMessage(enabled ? "Joystick control enabled" : "Joystick control disabled");
}

void Backend::setJoystickMaxThrottle(qreal max)
{
    if (max < 0.0) max = 0.0;
    if (max > 1.0) max = 1.0;
    m_joystick.set_max_throttle(static_cast<float>(max));
    emit logMessage(QStringLiteral("Joystick max throttle set to %1%").arg(max * 100.0));
}

bool Backend::isJoystickConnected() const
{
    return input_get_state().connected;
}

void Backend::updateJoystick()
{
    if (!m_connection.is_connected() || !m_mavlinkReady) {
        return;
    }

    // Isolated motor-test mode: never allow joystick RC override mixing
    // to fight with MAV_CMD_DO_MOTOR_TEST commands.
    if (m_motorTestActive) {
        return;
    }
    
    // Update SDL joystick state
    input_update();
    
    const ControllerState& state = input_get_state();
    
    // Only send commands if armed and joystick enabled
    if (m_telemetry.armed && m_joystick.is_enabled()) {
        ensureRov();
        if (m_rov) {
            m_joystick.update(m_rov, state);
        }
    }
}

void Backend::sendRCHeartbeat()
{
    if (!m_connection.is_connected() || !m_mavlinkReady) {
        return;
    }
    
    ensureRov();
    if (!m_rov) {
        return;
    }
    
    // Always send a MAVLink GCS heartbeat to avoid GCS failsafe disarm.
    m_rov->sendHeartbeat();

    // If disarmed, do not auto-force-arm from background loops.
    // Arming/disarming must be explicit to avoid state oscillation.
    if (!m_telemetry.armed) return;

    // ArduSub motor test expects periodic refreshes (QGC-style behavior).
    // Keep sending while user holds a non-zero motor-test throttle.
    if (m_motorTestActive && m_activeMotorThrottle > 0.0) {
        m_rov->setMotorThrottle(static_cast<uint8_t>(m_activeMotorIndex),
                                static_cast<float>(m_activeMotorThrottle));
    }
}

void Backend::updateCooldownDisplay()
{
    // Emit signal to update UI while cooldown is active
    if (motorTestCooldownMs() > 0) {
        emit motorTestCooldownChanged();
    }
}

void Backend::setCameraUrl(const QString &url)
{
    if (m_cameraUrl != url) {
        m_cameraUrl = url;
        emit cameraUrlChanged();
        emit logMessage(QStringLiteral("Camera URL set to: %1").arg(url));
    }
}

void Backend::connectCamera()
{
    m_videoProvider.connectToCamera(m_cameraUrl);
    emit logMessage(QStringLiteral("Connecting to camera: %1").arg(m_cameraUrl));
}

void Backend::disconnectCamera()
{
    m_videoProvider.disconnect();
    emit logMessage("Camera disconnected");
}
