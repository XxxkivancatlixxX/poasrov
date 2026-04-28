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

ROV::ROV(int sock, sockaddr_in target) 
    : sock(sock), target(target) {
    fprintf(stderr, "DEBUG ROV: Constructor called with socket=%d\n", sock);
}

void ROV::sendCommandLong(uint16_t command, float param1, float param2, 
                          float param3, float param4, float param5,
                          float param6, float param7) {
    if (command != MAV_CMD_DO_MOTOR_TEST) {
        fprintf(stderr,
                "DEBUG ROV: sendCommandLong cmd=%u p1=%.2f p2=%.2f p3=%.2f p4=%.2f p5=%.2f p6=%.2f p7=%.2f sock=%d\n",
                command, param1, param2, param3, param4, param5, param6, param7, sock);
    }
    
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
    if (command != MAV_CMD_DO_MOTOR_TEST) {
        fprintf(stderr, "DEBUG ROV: Packed %d bytes, sending via socket\n", len);
    }
    
    int result = sendto(sock, buf, len, 0, (sockaddr*)&target, sizeof(target));
    if (command != MAV_CMD_DO_MOTOR_TEST) {
        fprintf(stderr, "DEBUG ROV: sendto returned %d\n", result);
    }
}

void ROV::sendRCChannelsOverride(const uint16_t channels[8]) {
    // Send RC_CHANNELS_OVERRIDE message for thruster control using MAVLink library
    std::vector<uint8_t> packet = parser.create_rc_channels_override(channels);

    fprintf(stderr, "DEBUG ROV: Sending RC_CHANNELS_OVERRIDE (%zu bytes)\n", packet.size());

    int result = sendto(sock, packet.data(), packet.size(), 0, (sockaddr*)&target, sizeof(target));
    fprintf(stderr, "DEBUG ROV: sendto returned %d\n", result);
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
    // Send individual throttle commands to all 8 thrusters
    uint16_t channels[8];
    
    for (int i = 0; i < 8; i++) {
        float throttle = throttles[i];
        if (throttle < 0.0f) throttle = 0.0f;
        if (throttle > 1.0f) throttle = 1.0f;
        
        channels[i] = 1100 + (uint16_t)(throttle * 800.0f);
    }
    
    fprintf(stderr, "DEBUG ROV: PWM values: [%d, %d, %d, %d, %d, %d, %d, %d]\n",
            channels[0], channels[1], channels[2], channels[3], 
            channels[4], channels[5], channels[6], channels[7]);
    sendRCChannelsOverride(channels);
}

void ROV::arm()      { sendArmCommand(1.0f); }
void ROV::disarm()   { sendArmCommand(0.0f); }
void ROV::forceArm() { sendArmCommand(1.0f, 2989.0f); }

