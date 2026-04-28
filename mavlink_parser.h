#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <libs/c_library_v2/common/mavlink.h>

// Telemetry data extracted from MAVLink messages

// MAVLink frame structure
struct MAVLinkFrame {
    uint8_t start_byte;      // 0xFE
    uint8_t payload_len;     // Length of payload (0-255)
    uint8_t seq;             // Message sequence
    uint8_t sysid;           // System ID
    uint8_t compid;          // Component ID
    uint8_t msgid;           // Message ID
    uint8_t payload[256];    // Payload
    uint8_t checksum_low;    // CRC low byte
    uint8_t checksum_high;   // CRC high byte
};

// Telemetry data extracted from MAVLink messages
struct MAVLinkTelemetry {
    // From ATTITUDE
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    
    // From SCALED_PRESSURE2 or equivalent
    float depth = 0.0f;
    float temperature = 0.0f;
    float pressure = 0.0f;
    
    // From BATTERY_STATUS
    float battery_voltage = 0.0f;
    float battery_current = 0.0f;
    int battery_percentage = 0;
    
    // Status
    uint8_t armed = 0;
    uint8_t mode = 0;

    // Latest STATUSTEXT event (edge-triggered)
    bool statustext_updated = false;
    uint8_t statustext_severity = 0;
    char statustext_text[51] = {};

    // Latest COMMAND_ACK event (edge-triggered)
    bool command_ack_updated = false;
    uint16_t command_ack_command = 0;
    uint8_t command_ack_result = 0;
};

class MAVLinkParser {
public:
    MAVLinkParser();
    
    // Try to parse a byte stream into MAVLink messages
    bool parse_byte(uint8_t byte, MAVLinkFrame& frame, MAVLinkTelemetry& telemetry);
    
    // Extract telemetry from specific message types
    bool extract_heartbeat(const mavlink_message_t& msg, MAVLinkTelemetry& telem);
    bool extract_attitude(const mavlink_message_t& msg, MAVLinkTelemetry& telem);
    bool extract_battery(const mavlink_message_t& msg, MAVLinkTelemetry& telem);
    bool extract_pressure(const mavlink_message_t& msg, MAVLinkTelemetry& telem);
    
    // Create MANUAL_CONTROL command
    std::vector<uint8_t> create_manual_control(int16_t x, int16_t y, int16_t z, int16_t r, uint16_t buttons);
    
    // Create RC_CHANNELS_OVERRIDE command for motor control (ArduSub thrusters)
    // channels array should have 8 elements (1000-1900 PWM values for thrusters 1-8)
    std::vector<uint8_t> create_rc_channels_override(const uint16_t channels[8]);
    
private:
    mavlink_status_t status{};
};