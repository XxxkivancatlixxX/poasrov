#include "motor_config.h"
#include <cmath>

static void simple_memset(void* ptr, int val, unsigned int size) {
    unsigned char* p = (unsigned char*)ptr;
    for (unsigned int i = 0; i < size; i++) {
        p[i] = (unsigned char)val;
    }
}

MotorConfigManager::MotorConfigManager() {
    simple_memset(&current_frame, 0, sizeof(current_frame));
    simple_memset(&mixing_matrix, 0, sizeof(mixing_matrix));
}

MotorConfigManager::~MotorConfigManager() {}

bool MotorConfigManager::init() {
    build_vectored_frame(current_frame);
    recalculate_mixing_matrix();
    return true;
}

bool MotorConfigManager::set_frame(FrameType frame) {
    switch (frame) {
        case FRAME_VECTORED:
            build_vectored_frame(current_frame);
            break;
        case FRAME_QUADCOPTER:
            build_quadcopter_frame(current_frame);
            break;
        case FRAME_HEXACOPTER:
            build_hexacopter_frame(current_frame);
            break;
        case FRAME_OCTOCOPTER:
            build_octocopter_frame(current_frame);
            break;
        default:
            return false;
    }
    recalculate_mixing_matrix();
    return true;
}

bool MotorConfigManager::load_config(const char* config_file) {
    return true;
}

bool MotorConfigManager::save_config(const char* config_file) {
    return true;
}

void MotorConfigManager::calculate_motor_commands(float roll, float pitch, float yaw, 
                                                    float throttle, float* motor_outputs) {
    for (uint8_t i = 0; i < current_frame.num_motors; i++) {
        float output = throttle;
        output += roll * mixing_matrix.roll_mix[i];
        output += pitch * mixing_matrix.pitch_mix[i];
        output += yaw * mixing_matrix.yaw_mix[i];
        
        if (current_frame.motors[i].reversed) {
            output = 1.0f - output;
        }
        
        output = (output < 0.0f) ? 0.0f : (output > 1.0f) ? 1.0f : output;
        motor_outputs[i] = output;
    }
}

void MotorConfigManager::set_motor_reversed(uint8_t motor_id, uint8_t reversed) {
    if (motor_id < current_frame.num_motors) {
        current_frame.motors[motor_id].reversed = reversed;
    }
}

uint8_t MotorConfigManager::get_motor_reversed(uint8_t motor_id) const {
    if (motor_id < current_frame.num_motors) {
        return current_frame.motors[motor_id].reversed;
    }
    return 0;
}

void MotorConfigManager::recalculate_mixing_matrix() {
    simple_memset(&mixing_matrix, 0, sizeof(mixing_matrix));
    
    for (uint8_t i = 0; i < current_frame.num_motors; i++) {
        float angle_rad = current_frame.motors[i].angle_deg * 3.14159f / 180.0f;
        
        mixing_matrix.roll_mix[i] = cosf(angle_rad);
        mixing_matrix.pitch_mix[i] = sinf(angle_rad);
        mixing_matrix.yaw_mix[i] = (i % 2 == 0) ? 1.0f : -1.0f;
        mixing_matrix.throttle_mix[i] = 1.0f;
    }
}

void MotorConfigManager::build_vectored_frame(FrameConfig& frame) {
    frame.frame_type = FRAME_VECTORED;
    frame.num_motors = 8;
    strncpy(frame.name, "Vectored ROV (8x motor)", 32);
    
    float angles[8] = {0, 45, 90, 135, 180, 225, 270, 315};
    for (uint8_t i = 0; i < 8; i++) {
        frame.motors[i].motor_id = i;
        frame.motors[i].angle_deg = angles[i];
        frame.motors[i].reversed = 0;
        frame.motors[i].thrust_factor = 1.0f;
    }
}

void MotorConfigManager::build_quadcopter_frame(FrameConfig& frame) {
    frame.frame_type = FRAME_QUADCOPTER;
    frame.num_motors = 4;
    strncpy(frame.name, "Quadcopter (4x motor X)", 32);
    
    float angles[4] = {45, 135, 225, 315};
    uint8_t reversed[4] = {1, 0, 1, 0};
    for (uint8_t i = 0; i < 4; i++) {
        frame.motors[i].motor_id = i;
        frame.motors[i].angle_deg = angles[i];
        frame.motors[i].reversed = reversed[i];
        frame.motors[i].thrust_factor = 1.0f;
    }
}

void MotorConfigManager::build_hexacopter_frame(FrameConfig& frame) {
    frame.frame_type = FRAME_HEXACOPTER;
    frame.num_motors = 6;
    strncpy(frame.name, "Hexacopter (6x motor)", 32);
    
    float angles[6] = {0, 60, 120, 180, 240, 300};
    uint8_t reversed[6] = {0, 1, 0, 1, 0, 1};
    for (uint8_t i = 0; i < 6; i++) {
        frame.motors[i].motor_id = i;
        frame.motors[i].angle_deg = angles[i];
        frame.motors[i].reversed = reversed[i];
        frame.motors[i].thrust_factor = 1.0f;
    }
}

void MotorConfigManager::build_octocopter_frame(FrameConfig& frame) {
    frame.frame_type = FRAME_OCTOCOPTER;
    frame.num_motors = 8;
    strncpy(frame.name, "Octocopter (8x motor)", 32);
    
    float angles[8] = {22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5};
    uint8_t reversed[8] = {0, 1, 0, 1, 0, 1, 0, 1};
    for (uint8_t i = 0; i < 8; i++) {
        frame.motors[i].motor_id = i;
        frame.motors[i].angle_deg = angles[i];
        frame.motors[i].reversed = reversed[i];
        frame.motors[i].thrust_factor = 1.0f;
    }
}
