#pragma once

#include "input.h"
#include "ROV.h"
#include "controller_config.h"
#include <cstdint>

// Maps joystick input to ROV motor control
class JoystickControl {
public:
    JoystickControl();
    
    // Update motor commands based on current joystick state
    // Returns true if commands were sent
    bool update(ROV* rov, const ControllerState& state);
    
    // Configuration
    void set_deadzone(float dz);
    void set_max_throttle(float max) { max_throttle = max; }
    void set_enabled(bool en) { enabled = en; }
    
    bool is_enabled() const { return enabled; }
    
    // Controller config access
    ControllerConfigManager& get_config() { return config_manager; }
    const ControllerConfigManager& get_config() const { return config_manager; }
    
private:
    void calculate_motor_mix(const ControllerState& state, float motors[8]);
    
    ControllerConfigManager config_manager;
    float max_throttle = 0.5f;  // Safety limit (0.0-1.0)
    bool enabled = false;
    
    // Last sent values to avoid spamming identical commands
    float last_motors[8] = {0};
};
