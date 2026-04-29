#include "mavlink_parser.h"
#include <cstring>
#include <cstdio>
#include <chrono>

namespace {
void appendDebugLog(const char *hypothesisId,
                    const char *location,
                    const char *message,
                    const char *data,
                    const char *runId = "pre-fix")
{
    FILE *f = std::fopen("/home/vujuvuju/rov/PCside/.cursor/debug.log", "a");
    if (!f) return;
    const auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();
    std::fprintf(
        f,
        "{\"id\":\"log_%lld_%s\",\"timestamp\":%lld,\"location\":\"%s\",\"message\":\"%s\","
        "\"data\":{%s},\"runId\":\"%s\",\"hypothesisId\":\"%s\"}\n",
        static_cast<long long>(ts), hypothesisId, static_cast<long long>(ts), location, message,
        data, runId, hypothesisId);
    std::fclose(f);
}
}

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
        case MAVLINK_MSG_ID_SCALED_PRESSURE:
        case MAVLINK_MSG_ID_SCALED_PRESSURE2:
            extract_pressure(msg, telemetry);
            break;
        case MAVLINK_MSG_ID_COMMAND_ACK: {
            const uint16_t command = mavlink_msg_command_ack_get_command(&msg);
            const uint8_t result = mavlink_msg_command_ack_get_result(&msg);
            // Only log failed commands (result != 0 and != 4=unsupported)
            if (result != 0 && result != 4) {
                fprintf(stderr, "MAVLink: Command %u failed with result %u\n", command, result);
            }
            telemetry.command_ack_updated = true;
            telemetry.command_ack_command = command;
            telemetry.command_ack_result = result;
            if (command == MAV_CMD_DO_MOTOR_TEST ||
                command == MAV_CMD_COMPONENT_ARM_DISARM ||
                command == MAV_CMD_ACTUATOR_TEST ||
                command == MAV_CMD_DO_SET_ACTUATOR) {
                char data[192];
                std::snprintf(
                    data,
                    sizeof(data),
                    "\"command\":%u,\"result\":%u,\"sourceSysId\":%u,\"sourceCompId\":%u",
                    command, result, msg.sysid, msg.compid);
                // #region agent log
                appendDebugLog(
                    (command == MAV_CMD_DO_MOTOR_TEST || command == MAV_CMD_ACTUATOR_TEST || command == MAV_CMD_DO_SET_ACTUATOR) ? "H8" : "H5",
                    "mavlink_parser.cpp:parse_byte",
                    command == MAV_CMD_DO_MOTOR_TEST ? "received COMMAND_ACK for DO_MOTOR_TEST"
                    : (command == MAV_CMD_ACTUATOR_TEST ? "received COMMAND_ACK for ACTUATOR_TEST"
                    : (command == MAV_CMD_DO_SET_ACTUATOR ? "received COMMAND_ACK for DO_SET_ACTUATOR"
                    : "received COMMAND_ACK for motor/arm command")),
                    data);
                // #endregion
            }
            break;
        }
        case MAVLINK_MSG_ID_STATUSTEXT: {
            char text[51] = {};
            mavlink_msg_statustext_get_text(&msg, text);
            const uint8_t severity = mavlink_msg_statustext_get_severity(&msg);
            fprintf(stderr, "DEBUG MAVLink: STATUSTEXT severity=%u text=%s\n", severity, text);
            telemetry.statustext_updated = true;
            telemetry.statustext_severity = severity;
            std::strncpy(telemetry.statustext_text, text, sizeof(telemetry.statustext_text) - 1);
            telemetry.statustext_text[sizeof(telemetry.statustext_text) - 1] = '\0';
            char data[192];
            std::snprintf(
                data,
                sizeof(data),
                "\"severity\":%u,\"sourceSysId\":%u,\"sourceCompId\":%u,\"text\":\"%s\"",
                severity, msg.sysid, msg.compid, text);
            // #region agent log
            appendDebugLog(
                "H8",
                "mavlink_parser.cpp:parse_byte",
                "received STATUSTEXT",
                data);
            // #endregion
            break;
        }
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

    // MAVLink ATTITUDE message returns radians, convert to degrees
    const float RAD_TO_DEG = 57.295779513f;  // 180/PI
    telem.roll  = mavlink_msg_attitude_get_roll(&msg) * RAD_TO_DEG;
    telem.pitch = mavlink_msg_attitude_get_pitch(&msg) * RAD_TO_DEG;
    telem.yaw   = mavlink_msg_attitude_get_yaw(&msg) * RAD_TO_DEG;

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
    // ArduSub can send either SCALED_PRESSURE or SCALED_PRESSURE2
    if (msg.msgid != MAVLINK_MSG_ID_SCALED_PRESSURE && 
        msg.msgid != MAVLINK_MSG_ID_SCALED_PRESSURE2) {
        return false;
    }

    float press_hpa;
    int16_t temp_cdeg;
    
    if (msg.msgid == MAVLINK_MSG_ID_SCALED_PRESSURE) {
        press_hpa = mavlink_msg_scaled_pressure_get_press_abs(&msg);
        temp_cdeg = mavlink_msg_scaled_pressure_get_temperature(&msg);
    } else {
        press_hpa = mavlink_msg_scaled_pressure2_get_press_abs(&msg);
        temp_cdeg = mavlink_msg_scaled_pressure2_get_temperature(&msg);
    }
    
    float pressure_pa = press_hpa * 100.0f;

    telem.pressure = pressure_pa;
    // Depth calculation: (P - P_atm) / (rho * g)
    // P_atm = 101325 Pa, rho = 1000 kg/m³, g = 9.80665 m/s²
    telem.depth = (pressure_pa - 101325.0f) / 9806.65f;
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

