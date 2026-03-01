#include "control_sender.h"
#include <cstring>

ControlSender::ControlSender() : m_motor_test_mode(false) {
    memset(&m_packet, 0, sizeof(m_packet));
    m_packet.packet_type = 1;  // Control packet type
}

ControlSender::~ControlSender() {
}

void ControlSender::set_motor_test_mode(const float motor_throttles[8], bool enabled) {
    m_motor_test_mode = enabled;
    
    if (enabled) {
        m_packet.motor_count = 8;
        for (int i = 0; i < 8; i++) {
            m_packet.motors[i].motor_id = i;
            m_packet.motors[i].throttle = motor_throttles[i];
            m_packet.motors[i].enabled = (motor_throttles[i] > 0.0f) ? 1 : 0;
        }
    } else {
        m_packet.motor_count = 0;
    }
}

void ControlSender::set_control_mode(const ControllerState& controller) {
    if (m_motor_test_mode) return;  // Don't override motor test mode
    
    m_packet.motor_count = 0;  // Indicate we're using normal control mode
    
    // Use right trigger as overall throttle for now
    // You can add roll/pitch/yaw control from joysticks later
    float throttle = controller.trigger_right;
    m_packet.motors[0].throttle = throttle;
    m_packet.motors[0].enabled = (throttle > 0.0f) ? 1 : 0;
}

void ControlSender::set_armed(bool armed) {
    m_packet.armed = armed ? 1 : 0;
}

void ControlSender::set_flight_mode(uint8_t mode) {
    m_packet.flight_mode = mode;
}

uint8_t ControlSender::calculate_checksum(const uint8_t* data, uint16_t len) const {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

std::vector<uint8_t> ControlSender::serialize() const {
    std::vector<uint8_t> buffer(sizeof(ControlPacket));
    uint8_t* data = buffer.data();
    
    // Copy packet data
    memcpy(data, &m_packet, sizeof(ControlPacket) - 1);  // Don't copy checksum yet
    
    // Calculate and set checksum
    data[sizeof(ControlPacket) - 1] = calculate_checksum(data, sizeof(ControlPacket) - 1);
    
    return buffer;
}
