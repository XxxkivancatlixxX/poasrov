#include "TelemetryReceiverQt.h"

TelemetryReceiverQt::TelemetryReceiverQt(QObject *parent)
    : QObject(parent)
{
}

void TelemetryReceiverQt::handleRawPacket(const QByteArray &data)
{
    TelemetryPacket pktParsed{};
    if (!m_parser.parse_packet(
            reinterpret_cast<const uint8_t*>(data.constData()),
            static_cast<uint16_t>(data.size()),
            pktParsed)) {
        // Invalid packet; ignore
        return;
    }

    // Map TelemetryPacket → RemoteTelemetryPacket
    RemoteTelemetryPacket remote{};
    remote.packet_type = pktParsed.packet_type;
    // Direct struct copy is safe because layouts match by design [19][21]
    remote.state.armed        = pktParsed.state.armed;
    remote.state.flight_mode  = pktParsed.state.flight_mode;
    remote.state.sensors      = {
        pktParsed.state.sensors.gyro_x,
        pktParsed.state.sensors.gyro_y,
        pktParsed.state.sensors.gyro_z,
        pktParsed.state.sensors.accel_x,
        pktParsed.state.sensors.accel_y,
        pktParsed.state.sensors.accel_z,
        pktParsed.state.sensors.mag_x,
        pktParsed.state.sensors.mag_y,
        pktParsed.state.sensors.mag_z,
        pktParsed.state.sensors.depth,
        pktParsed.state.sensors.temperature,
        pktParsed.state.sensors.pressure
    };
    remote.state.battery      = {
        pktParsed.state.battery.voltage,
        pktParsed.state.battery.current,
        pktParsed.state.battery.capacity_mah,
        pktParsed.state.battery.percentage
    };
    remote.state.camera       = {
        pktParsed.state.camera.camera_type,
        pktParsed.state.camera.servo_min_pwm,
        pktParsed.state.camera.servo_max_pwm,
        pktParsed.state.camera.gimbal_type
    };
    remote.state.water        = {
        pktParsed.state.water.pressure_offset,
        pktParsed.state.water.temp_offset,
        pktParsed.state.water.salinity_type
    };
    remote.state.pid_tuning   = {
        pktParsed.state.pid_tuning.roll_p,  pktParsed.state.pid_tuning.roll_i,  pktParsed.state.pid_tuning.roll_d,
        pktParsed.state.pid_tuning.pitch_p, pktParsed.state.pid_tuning.pitch_i, pktParsed.state.pid_tuning.pitch_d,
        pktParsed.state.pid_tuning.yaw_p,   pktParsed.state.pid_tuning.yaw_i,   pktParsed.state.pid_tuning.yaw_d,
        pktParsed.state.pid_tuning.depth_p, pktParsed.state.pid_tuning.depth_i, pktParsed.state.pid_tuning.depth_d
    };
    remote.state.roll  = pktParsed.state.roll;
    remote.state.pitch = pktParsed.state.pitch;
    remote.state.yaw   = pktParsed.state.yaw;

    // Update receiver state and emit Qt signal
    m_receiver.update_from_packet(remote);
    emit stateUpdated(m_receiver.get_state());
}
