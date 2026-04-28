#include "telemetry_parser.h"
#include "mavlink_parser.h"
#include <cstring>
#include <cstdio>

static MAVLinkParser g_mavlink_parser;

TelemetryParser::TelemetryParser() {
    memset(buffer, 0, sizeof(buffer));
}

bool TelemetryParser::parse_byte(uint8_t byte) {
    buffer[buffer_idx++] = byte;
    
    if (buffer_idx >= sizeof(buffer)) {
        buffer_idx = 0;
        return false;
    }
    
    // On first byte, detect protocol
    if (buffer_idx == 1) {
        // MAVLink v1 (0xFE) and v2 (0xFD) start bytes
        if (byte == 0xFE || byte == 0xFD) {
            is_mavlink = true;
        } else if (byte == 2) {
            // Custom binary protocol (not implemented yet on firmware side)
            is_mavlink = false;
        } else {
            // Unknown start byte, reset parser
            buffer_idx = 0;
            return false;
        }
    }
    
    if (is_mavlink) {
        // MAVLink: try to parse as MAVLink frame
        MAVLinkFrame frame;
        if (g_mavlink_parser.parse_byte(byte, frame, mavlink_telemetry)) {

            // Mark that we have seen at least one HEARTBEAT on this link
            if (frame.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                mavlink_heartbeat_seen = true;
            }
            
            // Convert MAVLink to our RobotState format
            last_packet.packet_type = 2;
            last_packet.state.armed = mavlink_telemetry.armed;
            last_packet.state.battery.voltage = mavlink_telemetry.battery_voltage;
            last_packet.state.battery.percentage = mavlink_telemetry.battery_percentage;
            last_packet.state.sensors.depth = mavlink_telemetry.depth;
            last_packet.state.sensors.temperature = mavlink_telemetry.temperature;
            last_packet.state.roll = mavlink_telemetry.roll;
            last_packet.state.pitch = mavlink_telemetry.pitch;
            last_packet.state.yaw = mavlink_telemetry.yaw;
            
            packet_complete = true;
            buffer_idx = 0;
            return true;
        }
    } else {
        // Custom binary: check if we have a complete packet
        if (buffer_idx >= sizeof(TelemetryPacket)) {
            if (parse_packet(buffer, sizeof(TelemetryPacket), last_packet)) {
                packet_complete = true;
                buffer_idx = 0;
                return true;
            }
            // Not a valid packet, shift and try again
            memmove(buffer, buffer + 1, buffer_idx - 1);
            buffer_idx--;
        }
    }
    
    return false;
}

bool TelemetryParser::get_packet(RobotState& state) {
    if (packet_complete) {
        state = last_packet.state;
        packet_complete = false;
        return true;
    }
    return false;
}

bool TelemetryParser::takeLatestStatusText(std::string& text, uint8_t& severity)
{
    if (!mavlink_telemetry.statustext_updated) {
        return false;
    }
    text = mavlink_telemetry.statustext_text;
    severity = mavlink_telemetry.statustext_severity;
    mavlink_telemetry.statustext_updated = false;
    return true;
}

bool TelemetryParser::takeLatestCommandAck(uint16_t& command, uint8_t& result)
{
    if (!mavlink_telemetry.command_ack_updated) {
        return false;
    }
    command = mavlink_telemetry.command_ack_command;
    result = mavlink_telemetry.command_ack_result;
    mavlink_telemetry.command_ack_updated = false;
    return true;
}

bool TelemetryParser::parse_packet(const uint8_t* data, uint16_t len, TelemetryPacket& packet) {
    if (len < sizeof(TelemetryPacket)) {
        return false;
    }
    
    if (data[0] != 2) {
        return false;
    }
    
    memcpy(&packet, data, sizeof(TelemetryPacket));
    return true;
}

uint8_t TelemetryParser::calculate_checksum(const uint8_t* data, uint16_t len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void TelemetryParser::reset() {
    buffer_idx = 0;
    packet_complete = false;
    is_mavlink = false;
    mavlink_heartbeat_seen = false;
    memset(buffer, 0, sizeof(buffer));
    memset(&last_packet, 0, sizeof(last_packet));
    mavlink_telemetry = MAVLinkTelemetry{};
}
