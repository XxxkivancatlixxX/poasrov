#include "pixhawk_control.h"
#include <cstring>
#include <cmath>
#include <cstdio>

PixhawkControl::PixhawkControl() : armed(false) {
    for (int i = 0; i < 8; ++i) {
        motor_throttles[i] = 0.0f;
    }
}

PixhawkControl::~PixhawkControl() {
}

bool PixhawkControl::init() {
    printf("Initializing Pixhawk motor control system\n");
    return true;
}

void PixhawkControl::set_motor_throttle(uint8_t motor_id, float throttle) {
    if (motor_id >= 8) return;
    motor_throttles[motor_id] = throttle;
    apply_motor_commands();
}

void PixhawkControl::set_all_motors(const float throttles[8]) {
    for (int i = 0; i < 8; ++i) {
        motor_throttles[i] = throttles[i];
    }
    apply_motor_commands();
}

void PixhawkControl::apply_motor_commands() {
    if (!armed) {
        return;
    }
    
    for (int i = 0; i < 8; ++i) {
        float throttle = motor_throttles[i];
        throttle = (throttle < 0.0f) ? 0.0f : (throttle > 1.0f) ? 1.0f : throttle;
        
        uint16_t pwm_value = 1100 + (int)(throttle * 800);
        printf("Motor %d: %.2f -> PWM %d\n", i, throttle, pwm_value);
    }
}

IMUData PixhawkControl::read_imu() {
    IMUData data;
    data.accel_x = 0.0f;
    data.accel_y = 0.0f;
    data.accel_z = 9.81f;
    data.gyro_x = 0.0f;
    data.gyro_y = 0.0f;
    data.gyro_z = 0.0f;
    data.mag_x = 0.0f;
    data.mag_y = 0.0f;
    data.mag_z = 0.0f;
    return data;
}

DepthData PixhawkControl::read_depth() {
    DepthData data;
    data.depth = 0.0f;
    data.pressure = 101325.0f;
    data.temperature = 20.0f;
    return data;
}

void PixhawkControl::arm() {
    armed = true;
    printf("System ARMED\n");
}

void PixhawkControl::disarm() {
    armed = false;
    for (int i = 0; i < 8; ++i) {
        motor_throttles[i] = 0.0f;
    }
    printf("System DISARMED\n");
}

bool PixhawkControl::is_armed() const {
    return armed;
}

float PixhawkControl::get_motor_throttle(uint8_t motor_id) const {
    if (motor_id >= 8) return 0.0f;
    return motor_throttles[motor_id];
}

