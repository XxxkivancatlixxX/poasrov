#include "joystick_control.h"
#include <cmath>
#include <cstring>

JoystickControl::JoystickControl() {
    // Initialize to neutral position (0.5 in 0-1 range = 1500 PWM)
    for (int i = 0; i < 8; i++) {
        last_motors[i] = 0.5f;
    }
    
    // Initialize controller config with default QGC profile
    config_manager.init();
}

void JoystickControl::set_deadzone(float dz) {
    config_manager.get_active_profile_mutable().deadzone = dz;
}

void JoystickControl::calculate_motor_mix(const ControllerState& state, float motors[8]) {
    // Prepare axis values array for config manager
    float axis_values[6] = {
        state.axis_left_x,      // AXIS_LEFT_X
        state.axis_left_y,      // AXIS_LEFT_Y
        state.axis_right_x,     // AXIS_RIGHT_X
        state.axis_right_y,     // AXIS_RIGHT_Y
        state.trigger_left,     // AXIS_TRIGGER_LEFT
        state.trigger_right     // AXIS_TRIGGER_RIGHT
    };
    
    // Use controller config to calculate motor outputs
    config_manager.calculate_motor_outputs(axis_values, motors);
    
    // Apply max throttle limit (scale from neutral)
    for (int i = 0; i < 8; i++) {
        float deviation = motors[i] - 0.5f;  // Distance from neutral
        if (std::fabs(deviation) > max_throttle * 0.5f) {
            float sign = (deviation > 0.0f) ? 1.0f : -1.0f;
            motors[i] = 0.5f + sign * max_throttle * 0.5f;
        }
    }
}

bool JoystickControl::update(ROV* rov, const ControllerState& state) {
    if (!enabled || !rov) {
        fprintf(stderr, "JoystickControl: Skipping - enabled=%d rov=%p\n", enabled, (void*)rov);
        return false;
    }
    
    float motors[8];
    
    // If controller disconnected, send stop command
    if (!state.connected) {
        std::memset(motors, 0, sizeof(motors));
        
        // Check if we need to send stop (motors were running)
        bool was_running = false;
        for (int i = 0; i < 8; i++) {
            if (std::fabs(last_motors[i] - 0.5f) > 0.01f) {  // 0.5 is neutral in 0-1 range
                was_running = true;
                break;
            }
        }
        
        if (was_running) {
            // Convert to neutral (0.5 in 0-1 range)
            for (int i = 0; i < 8; i++) {
                motors[i] = 0.5f;
                last_motors[i] = 0.5f;
            }
            rov->setMotorThrottles(motors);
            return true;
        }
        return false;
    }
    
    // Debug: Print input state periodically
    static int input_debug = 0;
    if (++input_debug % 50 == 0) {
        fprintf(stderr, "JoystickControl: Input LX=%.2f LY=%.2f RX=%.2f TL=%.2f TR=%.2f\n",
                state.axis_left_x, state.axis_left_y, state.axis_right_x,
                state.trigger_left, state.trigger_right);
    }
    
    calculate_motor_mix(state, motors);
    
    // Debug: Print calculated motors
    if (input_debug % 50 == 0) {
        fprintf(stderr, "JoystickControl: Calculated motors: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n",
                motors[0], motors[1], motors[2], motors[3], motors[4], motors[5], motors[6], motors[7]);
    }
    
    // MANUAL_CONTROL messages MUST be sent continuously (like QGC does)
    // ArduSub expects regular updates or it will timeout and say "Lost manual control"
    // We send every update, not just on change
    
    // Update last values
    for (int i = 0; i < 8; i++) {
        last_motors[i] = motors[i];
    }
    
    // Debug output (only when values are non-neutral)
    bool non_neutral = false;
    for (int i = 0; i < 8; i++) {
        if (std::fabs(motors[i] - 0.5f) > 0.01f) {
            non_neutral = true;
            break;
        }
    }
    
    if (non_neutral || input_debug % 100 == 0) {
        fprintf(stderr, "DEBUG JoystickControl: Sending motors: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n",
                motors[0], motors[1], motors[2], motors[3], motors[4], motors[5], motors[6], motors[7]);
    }
    
    // Send via MAVLink (ROV class handles MANUAL_CONTROL)
    rov->setMotorThrottles(motors);
    
    return true;
}
