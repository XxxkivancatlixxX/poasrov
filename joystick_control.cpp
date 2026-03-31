#include "joystick_control.h"
#include <cmath>
#include <cstring>

JoystickControl::JoystickControl() {
    std::memset(last_motors, 0, sizeof(last_motors));
}

float JoystickControl::apply_deadzone(float value) {
    if (std::fabs(value) < deadzone) {
        return 0.0f;
    }
    // Scale to full range after deadzone
    float sign = (value > 0.0f) ? 1.0f : -1.0f;
    return sign * (std::fabs(value) - deadzone) / (1.0f - deadzone);
}

void JoystickControl::calculate_motor_mix(const ControllerState& state, float motors[8]) {
    // Apply deadzone to all axes
    float forward = -apply_deadzone(state.axis_left_y);   // Forward/backward
    float strafe = apply_deadzone(state.axis_left_x);     // Left/right strafe
    float yaw = apply_deadzone(state.axis_right_x);       // Rotation
    float vertical = apply_deadzone(state.trigger_right - state.trigger_left); // Up/down
    
    // Standard ROV motor configuration (vectored thrusters)
    // Motors 0-3: horizontal (forward/strafe/yaw)
    // Motors 4-7: vertical (up/down)
    
    // Horizontal thrusters (simplified 4-motor X configuration)
    motors[0] = forward + strafe + yaw;  // Front-right
    motors[1] = forward - strafe - yaw;  // Front-left
    motors[2] = forward - strafe + yaw;  // Rear-right
    motors[3] = forward + strafe - yaw;  // Rear-left
    
    // Vertical thrusters (all same for vertical movement)
    motors[4] = vertical;
    motors[5] = vertical;
    motors[6] = vertical;
    motors[7] = vertical;
    
    // Clamp and apply max throttle limit
    for (int i = 0; i < 8; i++) {
        if (motors[i] > max_throttle) motors[i] = max_throttle;
        if (motors[i] < -max_throttle) motors[i] = -max_throttle;
        
        // Convert from -1..1 to 0..1 range for ROV class
        motors[i] = (motors[i] + 1.0f) * 0.5f;
    }
}

bool JoystickControl::update(ROV* rov, const ControllerState& state) {
    if (!enabled || !rov || !state.connected) {
        return false;
    }
    
    float motors[8];
    calculate_motor_mix(state, motors);
    
    // Check if values changed significantly (avoid spamming)
    bool changed = false;
    for (int i = 0; i < 8; i++) {
        if (std::fabs(motors[i] - last_motors[i]) > 0.01f) {
            changed = true;
            break;
        }
    }
    
    if (!changed) {
        return false;
    }
    
    // Update last values and send
    for (int i = 0; i < 8; i++) {
        last_motors[i] = motors[i];
    }
    
    // Send via MAVLink (ROV class handles RC_CHANNELS_OVERRIDE)
    rov->setMotorThrottles(motors);
    
    return true;
}
