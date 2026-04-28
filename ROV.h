#pragma once
#include <libs/c_library_v2/common/mavlink.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "mavlink_parser.h"

class ROV {
public:
    ROV(int sock, sockaddr_in target);
    void arm();
    void disarm();
    void forceArm();   // bypasses pre-arm checks
    
    // ESC motor/thruster control (0-7 thrusters, 0.0-1.0 throttle)
    // For ArduSub: uses RC_CHANNELS_OVERRIDE (channels 1-8)
    void setMotorThrottle(uint8_t motor_id, float throttle);
    void setAllMotorThrottle(float throttle);  // Set all 8 motors to same throttle
    void setMotorThrottles(const float throttles[8]);  // Set all 8 motors individually
    void sendRCChannelsOverride(const uint16_t channels[8]);  // Direct RC override
    void sendHeartbeat();  // MAVLink GCS heartbeat

private:
    int sock;
    sockaddr_in target;
    MAVLinkParser parser;
    
    void sendArmCommand(float armParam, float forceParam = 0.0f);
    void sendCommandLong(uint16_t command, float param1, float param2 = 0.0f, 
                         float param3 = 0.0f, float param4 = 0.0f, float param5 = 0.0f,
                         float param6 = 0.0f, float param7 = 0.0f);
};
