#include "mission_control.h"
#include <cstdio>
#include <cstring>

MissionControl::MissionControl() : pixhawk_ptr(nullptr) {
    std::memset(&current_state, 0, sizeof(current_state));
}

MissionControl::~MissionControl() {
}

bool MissionControl::init(PixhawkControl* pixhawk) {
    pixhawk_ptr = pixhawk;
    return pixhawk_ptr != nullptr;
}

void MissionControl::handle_gui_command(const ControlPacket& cmd) {
    if (!pixhawk_ptr) return;
    
    printf("Received control packet with %d motors\n", cmd.motor_count);
    
    for (int i = 0; i < cmd.motor_count && i < 8; ++i) {
        uint8_t motor_id = cmd.motors[i].motor_id;
        float throttle = cmd.motors[i].throttle;
        
        if (cmd.motors[i].enabled) {
            pixhawk_ptr->set_motor_throttle(motor_id, throttle);
            printf("Motor %d: %.3f\n", motor_id, throttle);
        }
    }
}

void MissionControl::update_telemetry() {
    update_state();
}

void MissionControl::update_state() {
    if (!pixhawk_ptr) return;
    
    IMUData imu = pixhawk_ptr->read_imu();
    DepthData depth = pixhawk_ptr->read_depth();
    
    current_state.armed = pixhawk_ptr->is_armed();
    current_state.depth = depth.depth;
    current_state.temperature = depth.temperature;
    current_state.accel_x = imu.accel_x;
    current_state.accel_y = imu.accel_y;
    current_state.accel_z = imu.accel_z;
    current_state.gyro_x = imu.gyro_x;
    current_state.gyro_y = imu.gyro_y;
    current_state.gyro_z = imu.gyro_z;
}

TelemetryPacket MissionControl::get_telemetry() {
    ProtocolHandler protocol;
    return protocol.create_telemetry_packet(current_state);
}

void MissionControl::arm_system() {
    if (pixhawk_ptr) {
        pixhawk_ptr->arm();
        printf("ARM COMMAND EXECUTED\n");
    }
}

void MissionControl::disarm_system() {
    if (pixhawk_ptr) {
        pixhawk_ptr->disarm();
        printf("DISARM COMMAND EXECUTED\n");
    }
}
