#pragma once

#include <cstdint>
#include <memory>
#include "telemetry_parser.h"
#include "mavlink_parser.h"

// RobotState is defined in telemetry_parser.h as alias to TelemetryRobotState

enum class ProtocolType {
    CUSTOM_BINARY,
    MAVLINK,
    AUTO_DETECT
};

class ProtocolHandler {
public:
    ProtocolHandler(ProtocolType type = ProtocolType::CUSTOM_BINARY);
    
    // Set which protocol to use
    void set_protocol(ProtocolType type);
    
    // Try to detect protocol from incoming byte
    // Returns true if a complete message was parsed
    bool handle_incoming_byte(uint8_t byte);
    
    // Get current protocol type
    ProtocolType get_protocol_type() const { return current_protocol; }
    
    // Access latest telemetry data from custom protocol
    const RobotState& get_robot_state() const { return *last_robot_state; }
    
    // Access latest telemetry data from MAVLink protocol
    const MAVLinkTelemetry& get_mavlink_telemetry() const { return *last_mavlink_telemetry; }
    
    // Check if we have new data available
    bool has_new_telemetry() const { return new_telemetry_available; }
    void clear_new_telemetry_flag() { new_telemetry_available = false; }
    
private:
    ProtocolType current_protocol;
    ProtocolType detected_protocol;
    bool protocol_locked = false;  // Once detected, don't change
    
    // Telemetry storage
    std::unique_ptr<RobotState> last_robot_state;
    std::unique_ptr<MAVLinkTelemetry> last_mavlink_telemetry;
    bool new_telemetry_available = false;
    
    // Try parsing as custom protocol
    bool try_custom_protocol(uint8_t byte);
    
    // Try parsing as MAVLink
    bool try_mavlink_protocol(uint8_t byte);
};
