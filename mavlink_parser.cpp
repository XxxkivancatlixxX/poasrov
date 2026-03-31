#include "mavlink_parser.h"
#include <cstring>

MAVLinkParser::MAVLinkParser() {
    std::memset(&status, 0, sizeof(status));
}

bool MAVLinkParser::parse_byte(uint8_t byte, MAVLinkFrame& frame, MAVLinkTelemetry& telemetry) {
    mavlink_message_t msg;

    // Use the official MAVLink parser (handles v1/v2, CRC, etc.)
    if (!mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
        return false;
    }

    // Fill simplified frame structure for callers that want raw info
    frame.start_byte   = msg.magic;
    frame.payload_len  = msg.len;
    frame.seq          = msg.seq;
    frame.sysid        = msg.sysid;
    frame.compid       = msg.compid;
    frame.msgid        = msg.msgid;
    std::memset(frame.payload, 0, sizeof(frame.payload));
    if (msg.len > 0) {
        // _MAV_PAYLOAD returns a char pointer; copy exactly msg.len bytes into our uint8_t buffer.
        const void* payload = _MAV_PAYLOAD(&msg);
        std::memcpy(frame.payload, payload, msg.len);
    }
    frame.checksum_low  = msg.ck[0];
    frame.checksum_high = msg.ck[1];

    // Update high‑level telemetry for selected messages
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            extract_heartbeat(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
            extract_attitude(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_BATTERY_STATUS:
            extract_battery(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_SCALED_PRESSURE2:
            extract_pressure(msg, telemetry);
            break;
        default:
            break;
    }

    return true;
}

bool MAVLinkParser::extract_heartbeat(const mavlink_message_t& msg, MAVLinkTelemetry& telem) {
    if (msg.msgid != MAVLINK_MSG_ID_HEARTBEAT) {
        return false;
    }

    uint8_t base_mode = mavlink_msg_heartbeat_get_base_mode(&msg);
    telem.armed = (base_mode & 0x80) ? 1 : 0;  // Bit 7 is ARMED
    return true;
}

bool MAVLinkParser::extract_attitude(const mavlink_message_t& msg, MAVLinkTelemetry& telem) {
    if (msg.msgid != MAVLINK_MSG_ID_ATTITUDE) {
        return false;
    }

    telem.roll  = mavlink_msg_attitude_get_roll(&msg);
    telem.pitch = mavlink_msg_attitude_get_pitch(&msg);
    telem.yaw   = mavlink_msg_attitude_get_yaw(&msg);

    return true;
}

bool MAVLinkParser::extract_battery(const mavlink_message_t& msg, MAVLinkTelemetry& telem) {
    if (msg.msgid != MAVLINK_MSG_ID_BATTERY_STATUS) {
        return false;
    }

    uint16_t voltages[10] = {};
    mavlink_msg_battery_status_get_voltages(&msg, voltages);

    // Compute average pack voltage from valid cells (values of UINT16_MAX are "ignore")
    uint32_t sum_mv = 0;
    uint8_t  count  = 0;
    for (uint8_t i = 0; i < 10; ++i) {
        if (voltages[i] != UINT16_MAX && voltages[i] != 0) {
            sum_mv += voltages[i];
            ++count;
        }
    }
    if (count > 0) {
        telem.battery_voltage = static_cast<float>(sum_mv) / (1000.0f * count);
    }

    int16_t current_cA = mavlink_msg_battery_status_get_current_battery(&msg);
    if (current_cA != INT16_MAX) {
        telem.battery_current = static_cast<float>(current_cA) / 100.0f;
    }

    int8_t remaining = mavlink_msg_battery_status_get_battery_remaining(&msg);
    telem.battery_percentage = remaining < 0 ? 0 : remaining;

    return true;
}

bool MAVLinkParser::extract_pressure(const mavlink_message_t& msg, MAVLinkTelemetry& telem) {
    if (msg.msgid != MAVLINK_MSG_ID_SCALED_PRESSURE2) {
        return false;
    }

    // SCALED_PRESSURE2 press_abs is in hPa according to MAVLink spec
    float press_hpa = mavlink_msg_scaled_pressure2_get_press_abs(&msg);
    float pressure_pa = press_hpa * 100.0f;

    telem.pressure = pressure_pa;
    telem.depth = (pressure_pa - 101325.0f) / 9806.65f;  // Approximate depth in meters

    int16_t temp_cdeg = mavlink_msg_scaled_pressure2_get_temperature(&msg);
    telem.temperature = static_cast<float>(temp_cdeg) / 100.0f;

    return true;
}

std::vector<uint8_t> MAVLinkParser::create_manual_control(int16_t x, int16_t y, int16_t z, int16_t r, uint16_t buttons) {
    mavlink_message_t msg;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

    // Map inputs directly into MANUAL_CONTROL ranges (-1000..1000)
    uint8_t target_system = 1;
    uint16_t buttons2 = 0;
    uint8_t enabled_extensions = 0;
    int16_t s = 0, t = 0, aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;

    mavlink_msg_manual_control_pack(
        255,                // GCS system id
        190,                // GCS component id
        &msg,
        target_system,
        x, y, z, r,
        buttons,
        buttons2,
        enabled_extensions,
        s, t, aux1, aux2, aux3, aux4, aux5, aux6
    );

    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
    return std::vector<uint8_t>(buffer, buffer + len);
}

std::vector<uint8_t> MAVLinkParser::create_rc_channels_override(const uint16_t channels[8]) {
    mavlink_message_t msg;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

    // Clamp PWM values to ArduSub range (1000-1900 µs)
    uint16_t ch[8];
    for (int i = 0; i < 8; ++i) {
        uint16_t pwm = channels[i];
        if (pwm < 1000) pwm = 1000;
        if (pwm > 1900) pwm = 1900;
        ch[i] = pwm;
    }

    uint8_t target_system = 1;
    uint8_t target_component = 1;

    // Unused channels 9-18: set to 0 (ignored)
    uint16_t ignore = 0;

    mavlink_msg_rc_channels_override_pack(
        255, 190,           // GCS system/component id
        &msg,
        target_system,
        target_component,
        ch[0], ch[1], ch[2], ch[3],
        ch[4], ch[5], ch[6], ch[7],
        ignore, ignore, ignore, ignore,
        ignore, ignore, ignore, ignore,
        ignore, ignore
    );

    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
    return std::vector<uint8_t>(buffer, buffer + len);
}

