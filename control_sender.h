#pragma once

#include "input.h"
#include <cstdint>
#include <vector>

// Mirror of firmware MotorCommand for PC side
struct MotorCommand {
    uint8_t motor_id;
    float throttle;
    uint8_t enabled;
};

// Mirror of firmware ControlPacket for PC side
struct ControlPacket {
    uint8_t packet_type;
    uint8_t motor_count;
    MotorCommand motors[8];
    uint8_t armed;
    uint8_t flight_mode;
    uint8_t checksum;
};

class ControlSender {
public:
    ControlSender();
    ~ControlSender();
    
    // Set motor test mode - directly control motor throttles
    void set_motor_test_mode(const float motor_throttles[8], bool enabled = true);
    
    // Set normal control mode - use controller input
    void set_control_mode(const ControllerState& controller);
    
    // Set armed state
    void set_armed(bool armed);
    
    // Set flight mode
    void set_flight_mode(uint8_t mode);
    
    // Get the packet to send
    const ControlPacket& get_packet() const { return m_packet; }
    
    // Serialize to bytes for transmission
    std::vector<uint8_t> serialize() const;
    
private:
    ControlPacket m_packet;
    bool m_motor_test_mode;
    uint8_t calculate_checksum(const uint8_t* data, uint16_t len) const;
};
