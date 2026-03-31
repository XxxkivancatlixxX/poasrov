#include "ROV.h"
#include <cstdio>
#include <cstring>

ROV::ROV(int sock, sockaddr_in target) 
    : sock(sock), target(target) {
    fprintf(stderr, "DEBUG ROV: Constructor called with socket=%d\n", sock);
}

void ROV::sendCommandLong(uint16_t command, float param1, float param2, 
                          float param3, float param4, float param5,
                          float param6, float param7) {
    fprintf(stderr, "DEBUG ROV: sendCommandLong called with command=%d, param1=%.2f, sock=%d\n", 
            command, param1, sock);
    
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    mavlink_msg_command_long_pack(
        255, 190,          // GCS sysid, compid
        &msg,
        1, 1,              // target sysid, compid
        command,
        0,                 // confirmation
        param1, param2, param3, param4, param5, param6, param7
    );

    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    fprintf(stderr, "DEBUG ROV: Packed %d bytes, sending via socket\n", len);
    
    int result = sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
    fprintf(stderr, "DEBUG ROV: sendto returned %d\n", result);
}

void ROV::sendRCChannelsOverride(const uint16_t channels[8]) {
    // Send RC_CHANNELS_OVERRIDE message for thruster control using MAVLink library
    std::vector<uint8_t> packet = parser.create_rc_channels_override(channels);

    fprintf(stderr, "DEBUG ROV: Sending RC_CHANNELS_OVERRIDE (%zu bytes)\n", packet.size());

    int result = sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&target, sizeof(target));
    fprintf(stderr, "DEBUG ROV: sendto returned %d\n", result);
}

void ROV::sendArmCommand(float armParam, float forceParam) {
    sendCommandLong(MAV_CMD_COMPONENT_ARM_DISARM, armParam, forceParam);
}

void ROV::setMotorThrottle(uint8_t motor_id, float throttle) {
    if (motor_id > 7) return;  // Only 8 motors (0-7)

    // Clamp throttle between 0-1
    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;
    
    // Convert to PWM value (1100-1900 for ArduSub - 1100 is minimum with prop spin)
    uint16_t pwm = 1100 + (uint16_t)(throttle * 800.0f);  // 1100 to 1900

    // Build channels array with all motors idle except the selected one
    uint16_t channels[8];
    for (int i = 0; i < 8; ++i) {
        channels[i] = 1100;
    }
    channels[motor_id] = pwm;

    fprintf(stderr, "DEBUG ROV: Motor %d throttle=%.2f -> PWM=%d\n", motor_id, throttle, pwm);
    sendRCChannelsOverride(channels);
}

void ROV::setAllMotorThrottle(float throttle) {
    // Send throttle command to all 8 thrusters via RC_CHANNELS_OVERRIDE
    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;
    
    // Convert to PWM value (1100-1900 for ArduSub)
    uint16_t pwm = 1100 + (uint16_t)(throttle * 800.0f);
    
    // Create channel array with all 8 channels set to same PWM
    uint16_t channels[8];
    for (int i = 0; i < 8; i++) {
        channels[i] = pwm;
    }
    
    fprintf(stderr, "DEBUG ROV: Setting all thrusters to throttle=%.2f (PWM=%d)\n", throttle, pwm);
    sendRCChannelsOverride(channels);
}

void ROV::setMotorThrottles(const float throttles[8]) {
    // Send individual throttle commands to all 8 thrusters
    uint16_t channels[8];
    
    for (int i = 0; i < 8; i++) {
        float throttle = throttles[i];
        if (throttle < 0.0f) throttle = 0.0f;
        if (throttle > 1.0f) throttle = 1.0f;
        
        channels[i] = 1100 + (uint16_t)(throttle * 800.0f);
    }
    
    fprintf(stderr, "DEBUG ROV: Setting individual motor throttles\n");
    sendRCChannelsOverride(channels);
}

void ROV::arm()      { sendArmCommand(1.0f); }
void ROV::disarm()   { sendArmCommand(0.0f); }
void ROV::forceArm() { sendArmCommand(1.0f, 2989.0f); }

