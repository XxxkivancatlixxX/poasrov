#include "controller_config.h"
#include <cmath>

ControllerConfigManager::ControllerConfigManager() {
    std::memset(&active_profile, 0, sizeof(active_profile));
}

ControllerConfigManager::~ControllerConfigManager() {}

bool ControllerConfigManager::init() {
    create_default_profile(active_profile);
    return true;
}

bool ControllerConfigManager::load_profile(const char* profile_name) {
    return true;
}

bool ControllerConfigManager::save_profile(const char* profile_name) {
    return true;
}

bool ControllerConfigManager::create_profile(const char* profile_name) {
    strncpy(active_profile.name, profile_name, 64);
    return true;
}

bool ControllerConfigManager::add_mapping(const ControlMapping& mapping) {
    if (active_profile.num_mappings < 32) {
        active_profile.mappings[active_profile.num_mappings++] = mapping;
        return true;
    }
    return false;
}

bool ControllerConfigManager::remove_mapping(uint8_t mapping_id) {
    if (mapping_id < active_profile.num_mappings) {
        for (uint8_t i = mapping_id; i < active_profile.num_mappings - 1; i++) {
            active_profile.mappings[i] = active_profile.mappings[i + 1];
        }
        active_profile.num_mappings--;
        return true;
    }
    return false;
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

void ControllerConfigManager::create_default_profile(ControllerProfile& profile) {
    strncpy(profile.name, "Default", 64);
    profile.deadzone = 0.15f;
    profile.expo = 0.2f;
    profile.active = 1;
    profile.num_mappings = 0;
}

void ControllerConfigManager::create_racing_profile(ControllerProfile& profile) {
    strncpy(profile.name, "Racing", 64);
    profile.deadzone = 0.05f;
    profile.expo = 0.5f;
    profile.active = 0;
    profile.num_mappings = 0;
}

void ControllerConfigManager::create_advanced_profile(ControllerProfile& profile) {
    strncpy(profile.name, "Advanced", 64);
    profile.deadzone = 0.0f;
    profile.expo = 0.8f;
    profile.active = 0;
    profile.num_mappings = 0;
}
