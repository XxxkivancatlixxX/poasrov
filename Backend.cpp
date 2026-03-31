// backend for za poasrov qground copy jahoy jahoyyyy

#include "Backend.h"
#include "input.h"

#include <QDebug>

Backend::Backend(QObject *parent)
    : QObject(parent)
{
    // Initialize SDL for joystick support
    input_init();
    
    m_pollTimer.setInterval(30); // ~33 Hz
    connect(&m_pollTimer, &QTimer::timeout, this, &Backend::pollTelemetry);
    m_pollTimer.start();
    
    m_joystickTimer.setInterval(50); // 20 Hz for joystick updates
    connect(&m_joystickTimer, &QTimer::timeout, this, &Backend::updateJoystick);
    m_joystickTimer.start();

    updateConnectionStatus();
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
    emit mavlinkReadyChanged();
    updateConnectionStatus();
}

void Backend::setArmed(bool on)
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;

    m_telemetry.armed = on;
    emit armedChanged();

    ensureRov();
    if (!m_rov) return;

    if (on) m_rov->arm();
    else    m_rov->disarm();
}
// SOOO SOMETHİNG HAPPENİNG IDK WHAT
void Backend::setMotorTest(int motorIndex, qreal throttle)
{
    if (!m_connection.is_connected()) return;
    if (!m_mavlinkReady) return;
    if (motorIndex < 0 || motorIndex > 7) return;

    if (throttle < 0.0) throttle = 0.0;
    if (throttle > 1.0) throttle = 1.0;

    ensureRov();
    if (!m_rov) return;

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
            RobotState rs;
            if (m_telemetryParser.get_packet(rs)) {
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
                emit armedChanged();

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
