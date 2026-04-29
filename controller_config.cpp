// configsel
#include "controller_config.h"
#include <cmath>
#include <cstdio>

ControllerConfigManager::ControllerConfigManager() {
    std::memset(&active_profile, 0, sizeof(active_profile));
}

ControllerConfigManager::~ControllerConfigManager() {}

bool ControllerConfigManager::init() {
    create_default_qgc_profile(active_profile);
    return true;
}

bool ControllerConfigManager::load_profile(const char* profile_name) {
    // TODO: Load from file
    return true;
}

bool ControllerConfigManager::save_profile(const char* profile_name) {
    // TODO: Save to file
    return true;
}

bool ControllerConfigManager::create_profile(const char* profile_name) {
    strncpy(active_profile.name, profile_name, 64);
    active_profile.num_motors = 8;
    return true;
}

bool ControllerConfigManager::add_motor_mapping(const MotorMapping& mapping) {
    if (active_profile.num_motor_mappings < 32) {
        active_profile.motor_mappings[active_profile.num_motor_mappings++] = mapping;
        return true;
    }
    return false;
}

bool ControllerConfigManager::remove_motor_mapping(uint8_t mapping_id) {
    if (mapping_id < active_profile.num_motor_mappings) {
        for (uint8_t i = mapping_id; i < active_profile.num_motor_mappings - 1; i++) {
            active_profile.motor_mappings[i] = active_profile.motor_mappings[i + 1];
        }
        active_profile.num_motor_mappings--;
        return true;
    }
    return false;
}

bool ControllerConfigManager::clear_motor_mappings(uint8_t motor_id) {
    uint8_t write_idx = 0;
    for (uint8_t i = 0; i < active_profile.num_motor_mappings; i++) {
        if (active_profile.motor_mappings[i].motor_id != motor_id) {
            if (write_idx != i) {
                active_profile.motor_mappings[write_idx] = active_profile.motor_mappings[i];
            }
            write_idx++;
        }
    }
    active_profile.num_motor_mappings = write_idx;
    return true;
}

void ControllerConfigManager::apply_deadzone(float& value, float deadzone) {
    if (value > -deadzone && value < deadzone) {
        value = 0.0f;
    } else if (value > deadzone) {
        value = (value - deadzone) / (1.0f - deadzone);
    } else {
        value = (value + deadzone) / (1.0f - deadzone);
    }
}

float ControllerConfigManager::apply_expo(float value, float expo) {
    if (expo == 0.0f) return value;
    return value * (expo * value * value + (1.0f - expo));
}

void ControllerConfigManager::calculate_motor_outputs(const float axis_values[6], float motor_outputs[8]) {
    
    for (int i = 0; i < 8; i++) {
        motor_outputs[i] = 0.5f;
    }
    
    
    float motor_contributions[8] = {0};
    
    for (uint8_t i = 0; i < active_profile.num_motor_mappings; i++) {
        const MotorMapping& mapping = active_profile.motor_mappings[i];
        
        if (!mapping.enabled || mapping.motor_id >= 8) {
            continue;
        }
        
        float input_value = 0.0f;
        
        // Get input value based on type
        if (mapping.input_type == INPUT_AXIS || mapping.input_type == INPUT_TRIGGER) {
            if (mapping.input_id < 6) {
                input_value = axis_values[mapping.input_id];
            }
        }
        
        
        apply_deadzone(input_value, active_profile.deadzone);
        
       
        input_value = apply_expo(input_value, active_profile.expo);
        
        
        if (mapping.inverted) {
            input_value = -input_value;
        }
        input_value *= mapping.scale;
        
        
        motor_contributions[mapping.motor_id] += input_value;
    }
    
    
    for (int i = 0; i < 8; i++) {
        motor_outputs[i] = 0.5f + (motor_contributions[i] * 0.5f);
        
        // Clamp to valid range
        if (motor_outputs[i] < 0.0f) motor_outputs[i] = 0.0f;
        if (motor_outputs[i] > 1.0f) motor_outputs[i] = 1.0f;
    }
}

void ControllerConfigManager::create_default_qgc_profile(ControllerProfile& profile) {
    strncpy(profile.name, "Simple Direct Control", 64);
    profile.deadzone = 0.1f;
    profile.expo = 0.0f;
    profile.active = 1;
    profile.num_motors = 8;
    profile.num_motor_mappings = 0;
    
 
    
    
    MotorMapping throttle = {2, INPUT_AXIS, AXIS_RIGHT_Y, -1.0f, false, true};
    
    
    MotorMapping yaw = {3, INPUT_AXIS, AXIS_LEFT_X, 1.0f, false, true};
    
    
    MotorMapping pitch = {4, INPUT_AXIS, AXIS_LEFT_Y, -1.0f, false, true};
    
    
    MotorMapping roll = {5, INPUT_AXIS, AXIS_RIGHT_X, 1.0f, false, true};
    
    // mappingsel
    profile.motor_mappings[profile.num_motor_mappings++] = throttle;
    profile.motor_mappings[profile.num_motor_mappings++] = yaw;
    profile.motor_mappings[profile.num_motor_mappings++] = pitch;
    profile.motor_mappings[profile.num_motor_mappings++] = roll;
}

void ControllerConfigManager::create_custom_profile(ControllerProfile& profile, const char* name) {
    strncpy(profile.name, name, 64);
    profile.deadzone = 0.1f;
    profile.expo = 0.0f;
    profile.active = 1;
    profile.num_motors = 8;
    profile.num_motor_mappings = 0;
}

void ControllerConfigManager::create_simple_mode_profile(ControllerProfile& profile) {
    strncpy(profile.name, "Simple Mode (No Motor Config)", 64);
    profile.deadzone = 0.15f;
    profile.expo = 0.0f;
    profile.active = 1;
    profile.num_motors = 8;
    profile.num_motor_mappings = 0;
    
    // Simple mode: Direct movement control without motor configuration
    // All motors get the same vectored thruster mixing
    // This is the "I just want to fly" mode
    
    // Motors 0-3: Horizontal (forward/strafe/yaw)
    // Forward: Left Stick Y (inverted)
    MotorMapping m0_fwd = {0, INPUT_AXIS, AXIS_LEFT_Y, -1.0f, false, true};
    MotorMapping m1_fwd = {1, INPUT_AXIS, AXIS_LEFT_Y, -1.0f, false, true};
    MotorMapping m2_fwd = {2, INPUT_AXIS, AXIS_LEFT_Y, -1.0f, false, true};
    MotorMapping m3_fwd = {3, INPUT_AXIS, AXIS_LEFT_Y, -1.0f, false, true};
    
    // Strafe: Left Stick X
    MotorMapping m0_strafe = {0, INPUT_AXIS, AXIS_LEFT_X, 1.0f, false, true};
    MotorMapping m1_strafe = {1, INPUT_AXIS, AXIS_LEFT_X, -1.0f, false, true};
    MotorMapping m2_strafe = {2, INPUT_AXIS, AXIS_LEFT_X, -1.0f, false, true};
    MotorMapping m3_strafe = {3, INPUT_AXIS, AXIS_LEFT_X, 1.0f, false, true};
    
    // Yaw: Right Stick X
    MotorMapping m0_yaw = {0, INPUT_AXIS, AXIS_RIGHT_X, 1.0f, false, true};
    MotorMapping m1_yaw = {1, INPUT_AXIS, AXIS_RIGHT_X, -1.0f, false, true};
    MotorMapping m2_yaw = {2, INPUT_AXIS, AXIS_RIGHT_X, 1.0f, false, true};
    MotorMapping m3_yaw = {3, INPUT_AXIS, AXIS_RIGHT_X, -1.0f, false, true};
    
    // Motors 4-7: Vertical (up/down)
    // Right Trigger = Up, Left Trigger = Down
    MotorMapping m4_up = {4, INPUT_TRIGGER, AXIS_TRIGGER_RIGHT, 1.0f, false, true};
    MotorMapping m4_down = {4, INPUT_TRIGGER, AXIS_TRIGGER_LEFT, -1.0f, false, true};
    MotorMapping m5_up = {5, INPUT_TRIGGER, AXIS_TRIGGER_RIGHT, 1.0f, false, true};
    MotorMapping m5_down = {5, INPUT_TRIGGER, AXIS_TRIGGER_LEFT, -1.0f, false, true};
    MotorMapping m6_up = {6, INPUT_TRIGGER, AXIS_TRIGGER_RIGHT, 1.0f, false, true};
    MotorMapping m6_down = {6, INPUT_TRIGGER, AXIS_TRIGGER_LEFT, -1.0f, false, true};
    MotorMapping m7_up = {7, INPUT_TRIGGER, AXIS_TRIGGER_RIGHT, 1.0f, false, true};
    MotorMapping m7_down = {7, INPUT_TRIGGER, AXIS_TRIGGER_LEFT, -1.0f, false, true};
    
    // Add all mappings
    profile.motor_mappings[profile.num_motor_mappings++] = m0_fwd;
    profile.motor_mappings[profile.num_motor_mappings++] = m0_strafe;
    profile.motor_mappings[profile.num_motor_mappings++] = m0_yaw;
    
    profile.motor_mappings[profile.num_motor_mappings++] = m1_fwd;
    profile.motor_mappings[profile.num_motor_mappings++] = m1_strafe;
    profile.motor_mappings[profile.num_motor_mappings++] = m1_yaw;
    
    profile.motor_mappings[profile.num_motor_mappings++] = m2_fwd;
    profile.motor_mappings[profile.num_motor_mappings++] = m2_strafe;
    profile.motor_mappings[profile.num_motor_mappings++] = m2_yaw;
    
    profile.motor_mappings[profile.num_motor_mappings++] = m3_fwd;
    profile.motor_mappings[profile.num_motor_mappings++] = m3_strafe;
    profile.motor_mappings[profile.num_motor_mappings++] = m3_yaw;
    
    profile.motor_mappings[profile.num_motor_mappings++] = m4_up;
    profile.motor_mappings[profile.num_motor_mappings++] = m4_down;
    profile.motor_mappings[profile.num_motor_mappings++] = m5_up;
    profile.motor_mappings[profile.num_motor_mappings++] = m5_down;
    profile.motor_mappings[profile.num_motor_mappings++] = m6_up;
    profile.motor_mappings[profile.num_motor_mappings++] = m6_down;
    profile.motor_mappings[profile.num_motor_mappings++] = m7_up;
    profile.motor_mappings[profile.num_motor_mappings++] = m7_down;
}
