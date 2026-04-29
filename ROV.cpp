// poasrov deafult rtsp is rtsp://192.168.1.2:8554/cam
#include "ROV.h"
#include <cstdio>
#include <cstring>
#include <chrono>
#include <limits>

namespace {
void appendDebugLog(const char *hypothesisId,
                    const char *location,
                    const char *message,
                    const char *data,
                    const char *runId = "pre-fix")
{
    FILE *f = std::fopen("/home/vujuvuju/rov/PCside/.cursor/debug.log", "a"); // TODO: make this a user input
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

ROV::ROV(int sock, sockaddr_in target) 
    : sock(sock), target(target) {
    // initROV
}

void ROV::sendCommandLong(uint16_t command, float param1, float param2, 
                          float param3, float param4, float param5,
                          float param6, float param7) {
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
    sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
}

void ROV::sendRCChannelsOverride(const uint16_t channels[8]) {
    // Send RC_CHANNELS_OVERRIDE message for thruster control using MAVLink library
    std::vector<uint8_t> packet = parser.create_rc_channels_override(channels);
    sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&target, sizeof(target));
}

void ROV::sendHeartbeat() {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    mavlink_msg_heartbeat_pack(
        255, 190, // GCS sysid, compid
        &msg,
        MAV_TYPE_GCS,
        MAV_AUTOPILOT_INVALID,
        0, // base_mode
        0, // custom_mode
        MAV_STATE_ACTIVE
    );

    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
}

void ROV::sendArmCommand(float armParam, float forceParam) {
    sendCommandLong(MAV_CMD_COMPONENT_ARM_DISARM, armParam, forceParam);
}

void ROV::setMotorThrottle(uint8_t motor_id, float throttle) {
    if (motor_id > 7) return;  // Only 8 motors (0-7)

    // Clamp throttle between 0-1
    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;

    // This firmware rejects DO_SET_ACTUATOR (result=3 unsupported) and RC override
    // drives mixer axes, not true per-motor output. Use DO_MOTOR_TEST in PWM mode.

    const float motor_number = static_cast<float>(motor_id); // Use motor_id directly (0-7) instead of motor_id+1
    const float throttle_type_percent = 0.0f; // MOTOR_TEST_THROTTLE_PERCENT
    const float test_percent = throttle * 100.0f; // 0..100
    const float timeout_sec = 120.0f; // 2 minutes timeout for inactivity
    const float motor_count = 1.0f; // test one motor
    const float test_order_board = 2.0f; // MOTOR_TEST_ORDER_BOARD (required by ArduSub)

    sendCommandLong(
        MAV_CMD_DO_MOTOR_TEST,
        motor_number,
        throttle_type_percent,
        test_percent,
        timeout_sec,
        motor_count,
        test_order_board,
        0.0f
    );
}

void ROV::setAllMotorThrottle(float throttle) {
    // Send command to all 8 channels via RC_CHANNELS_OVERRIDE.
    // For test use, treat 0.0 as stop/neutral (1500us) and 1.0 as max forward (1900us).
    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;
    
    // Neutral-centered output: 1500..1900
    uint16_t pwm = 1500 + (uint16_t)(throttle * 400.0f);
    
    // Create channel array with all 8 channels set to same PWM
    uint16_t channels[8];
    for (int i = 0; i < 8; i++) {
        channels[i] = pwm;
    }
    
    fprintf(stderr, "DEBUG ROV: Setting all channels throttle=%.2f (PWM=%d)\n", throttle, pwm);
    sendRCChannelsOverride(channels);
}

void ROV::setMotorThrottles(const float throttles[8]) {
    // Convert to RC_CHANNELS_OVERRIDE
    // throttles[] is in 0.0-1.0 range where 0.5 = neutral
    // PWM should be 1100-1900 where 1500 = neutral
    
    uint16_t channels[8];
    
    for (int i = 0; i < 8; i++) {
        // Convert from 0.0-1.0 (0.5=neutral) to PWM 1100-1900 (1500=neutral)
        // Formula: PWM = 1500 + (input - 0.5) * 800
        float centered = throttles[i] - 0.5f;  // -0.5 to +0.5
        int16_t pwm = 1500 + (int16_t)(centered * 800.0f);
        
        // Clamp to valid range
        if (pwm < 1100) pwm = 1100;
        if (pwm > 1900) pwm = 1900;
        
        channels[i] = (uint16_t)pwm;
    }
    
    sendRCChannelsOverride(channels);
    
    // Log RC values occasionally for debugging
    static int debug_counter = 0;
    if (++debug_counter % 100 == 0) {  // Every 2 seconds
        fprintf(stderr, "RC: ch3=%d ch4=%d ch5=%d ch6=%d\n", 
                channels[2], channels[3], channels[4], channels[5]);
    }
}

void ROV::arm()      { sendArmCommand(1.0f); }
void ROV::disarm()   { sendArmCommand(0.0f); }
void ROV::forceArm() { sendArmCommand(1.0f, 2989.0f); }

void ROV::setFlightMode(uint32_t custom_mode) {
    // Set flight mode using MAV_CMD_DO_SET_MODE
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    
    mavlink_msg_command_long_pack(
        255, 190,          // GCS sysid, compid
        &msg,
        1, 1,              // target sysid, compid
        MAV_CMD_DO_SET_MODE,
        0,                 // confirmation
        MAV_MODE_FLAG_CUSTOM_MODE_ENABLED,  // param1: mode
        custom_mode,       // param2: custom mode
        0, 0, 0, 0, 0
    );
    
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
}

void ROV::setParameter(const char* param_id, float value) {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    
    mavlink_param_set_t param_set;
    param_set.target_system = 1;
    param_set.target_component = 1;
    param_set.param_value = value;
    param_set.param_type = MAV_PARAM_TYPE_REAL32;
    
    // Copy parameter name (max 16 chars)
    strncpy(param_set.param_id, param_id, 16);
    param_set.param_id[15] = '\0';
    
    mavlink_msg_param_set_encode(255, 190, &msg, &param_set);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    
    fprintf(stderr, "DEBUG ROV: Setting parameter %s = %.2f\n", param_id, value);
    sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
}

void ROV::requestParameter(const char* param_id) {
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    
    mavlink_param_request_read_t req;
    req.target_system = 1;
    req.target_component = 1;
    strncpy(req.param_id, param_id, 16);
    req.param_id[15] = '\0';
    req.param_index = -1;
    
    mavlink_msg_param_request_read_encode(255, 190, &msg, &req);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    
    fprintf(stderr, "DEBUG ROV: Requesting parameter %s\n", param_id);
    sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
}

void ROV::reverseMotor(int motor_num) {
    // Reverse a motor by setting MOT_n_DIRECTION to -1
    // motor_num is 1-8 (ArduSub motor numbering)
    if (motor_num < 1 || motor_num > 8) return;
    
    char param_name[17];
    snprintf(param_name, sizeof(param_name), "MOT_%d_DIRECTION", motor_num);
    
    fprintf(stderr, "DEBUG ROV: Reversing motor %d via parameter %s\n", motor_num, param_name);
    setParameter(param_name, -1.0f);
}

