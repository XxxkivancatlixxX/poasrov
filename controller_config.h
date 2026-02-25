#pragma once

#include <cstdint>
#include <cstring>

enum ControlInputType {
    INPUT_AXIS = 0,
    INPUT_BUTTON = 1,
    INPUT_POV = 2
};

enum ControlOutputType {
    OUTPUT_ROLL = 0,
    OUTPUT_PITCH = 1,
    OUTPUT_YAW = 2,
    OUTPUT_THROTTLE = 3,
    OUTPUT_ARM = 4,
    OUTPUT_MODE = 5,
    OUTPUT_CUSTOM = 6
};

struct ControlMapping {
    ControlInputType input_type;
    uint8_t input_id;
    uint8_t input_index;
    ControlOutputType output_type;
    float scale;
    float offset;
    uint8_t inverted;
};

struct ControllerProfile {
    char name[64];
    ControlMapping mappings[32];
    uint8_t num_mappings;
    float deadzone;
    float expo;
    uint8_t active;
};

class ControllerConfigManager {
public:
    ControllerConfigManager();
    ~ControllerConfigManager();
    
    bool init();
    bool load_profile(const char* profile_name);
    bool save_profile(const char* profile_name);
    
    bool create_profile(const char* profile_name);
    bool add_mapping(const ControlMapping& mapping);
    bool remove_mapping(uint8_t mapping_id);
    
    void apply_deadzone(float& value, float deadzone);
    float apply_expo(float value, float expo);
    
    const ControllerProfile& get_active_profile() const { return active_profile; }
    
    static void create_default_profile(ControllerProfile& profile);
    static void create_racing_profile(ControllerProfile& profile);
    static void create_advanced_profile(ControllerProfile& profile);
    
private:
    ControllerProfile active_profile;
};

