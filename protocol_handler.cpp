#include "protocol_handler.h"
#include "telemetry_parser.h"
#include "mavlink_parser.h"
#include "controller_config.h"
#include <cstring>

static TelemetryParser g_custom_parser;
static MAVLinkParser g_mavlink_parser;

ProtocolHandler::ProtocolHandler(ProtocolType type) 
    : current_protocol(type), detected_protocol(ProtocolType::AUTO_DETECT) {
    last_robot_state = std::make_unique<RobotState>();
    last_mavlink_telemetry = std::make_unique<MAVLinkTelemetry>();
}

void ProtocolHandler::set_protocol(ProtocolType type) {
    if (type != ProtocolType::AUTO_DETECT) {
        current_protocol = type;
        detected_protocol = type;
        protocol_locked = true;
    } else {
        protocol_locked = false;
    }
}

bool ProtocolHandler::handle_incoming_byte(uint8_t byte) {
    if (protocol_locked) {
        // We know which protocol to use
        if (current_protocol == ProtocolType::CUSTOM_BINARY) {
            return try_custom_protocol(byte);
        } else {
            return try_mavlink_protocol(byte);
        }
    } else {
        // Auto-detect mode: try to figure out which protocol this is
        // Custom protocol packet type bytes are 1-255 (not 0xFE)
        // MAVLink starts with 0xFE
        
        if (byte == 0xFE) {
            current_protocol = ProtocolType::MAVLINK;
            detected_protocol = ProtocolType::MAVLINK;
            protocol_locked = true;
            return try_mavlink_protocol(byte);
        } else {
            // Assume custom protocol for now
            // In a real system, you might want more sophisticated detection
            if (detected_protocol != ProtocolType::CUSTOM_BINARY) {
                detected_protocol = ProtocolType::CUSTOM_BINARY;
                current_protocol = ProtocolType::CUSTOM_BINARY;
                protocol_locked = true;
            }
            return try_custom_protocol(byte);
        }
    }
    return false;
}

bool ProtocolHandler::try_custom_protocol(uint8_t byte) {
    // Use existing telemetry parser
    if (g_custom_parser.parse_byte(byte)) {
        // Got a complete custom packet
        if (g_custom_parser.get_packet(*last_robot_state)) {
            new_telemetry_available = true;
            return true;
        }
    }
    return false;
}

bool ProtocolHandler::try_mavlink_protocol(uint8_t byte) {
    MAVLinkFrame frame;
    MAVLinkTelemetry telem;
    
    if (g_mavlink_parser.parse_byte(byte, frame, telem)) {
        // Got a complete MAVLink frame
        *last_mavlink_telemetry = telem;
        new_telemetry_available = true;
        return true;
    }
    return false;
}
