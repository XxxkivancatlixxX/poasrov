#pragma once

#include <cstdint>
#include <cstring>

// Input sources for controller mapping
enum ControlInputType {
    INPUT_AXIS = 0,
    INPUT_BUTTON = 1,
    INPUT_POV = 2,
    INPUT_TRIGGER = 3
};

// Specific axis identifiers
enum AxisInput {
    AXIS_LEFT_X = 0,
    AXIS_LEFT_Y = 1,
    AXIS_RIGHT_X = 2,
    AXIS_RIGHT_Y = 3,
    AXIS_TRIGGER_LEFT = 4,
    AXIS_TRIGGER_RIGHT = 5
};

// Motor mapping configuration
struct MotorMapping {
    uint8_t motor_id;           // 0-7 for 8 motors
    ControlInputType input_type;
    uint8_t input_id;           // Which axis/button (use AxisInput enum for axes)
    float scale;                // Multiplier for input value
    bool inverted;              // Invert the input
    bool enabled;               // Is this mapping active
};

// Controller profile with motor mappings
struct ControllerProfile {
    char name[64];
    MotorMapping motor_mappings[32];  // Support multiple inputs per motor
    uint8_t num_motor_mappings;
    float deadzone;
    float expo;
    uint8_t active;
    uint8_t num_motors;         // Total motors (default 8)
};

class ControllerConfigManager {
public:
    ControllerConfigManager();
    ~ControllerConfigManager();
    
    bool init();
    bool load_profile(const char* profile_name);
    bool save_profile(const char* profile_name);
    
    bool create_profile(const char* profile_name);
    bool add_motor_mapping(const MotorMapping& mapping);
    bool remove_motor_mapping(uint8_t mapping_id);
    bool clear_motor_mappings(uint8_t motor_id);
    
    void apply_deadzone(float& value, float deadzone);
    float apply_expo(float value, float expo);
    
    const ControllerProfile& get_active_profile() const { return active_profile; }
    ControllerProfile& get_active_profile_mutable() { return active_profile; }
    
    // Calculate motor values from controller state
    void calculate_motor_outputs(const float axis_values[6], float motor_outputs[8]);
    
    // Default profiles
    static void create_default_qgc_profile(ControllerProfile& profile);
    static void create_simple_mode_profile(ControllerProfile& profile);
    static void create_custom_profile(ControllerProfile& profile, const char* name);
    
private:
    ControllerProfile active_profile;
};

