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
    
    // TODO: Replace with actual I2C/SPI reads from MPU6050, HMC5883L, etc.
    // For now: return realistic simulated data so UI has something to display
    static float sim_time = 0.0f;
    sim_time += 0.01f;
    
    // Simulate gyro movement
    data.gyro_x = 0.5f * sinf(sim_time / 10.0f);
    data.gyro_y = 0.3f * cosf(sim_time / 15.0f);
    data.gyro_z = 0.2f * sinf(sim_time / 20.0f);
    
    // Simulate acceleration with gravity
    data.accel_x = 0.2f * sinf(sim_time / 8.0f);
    data.accel_y = 0.15f * cosf(sim_time / 12.0f);
    data.accel_z = 9.81f + 0.1f * sinf(sim_time / 10.0f);
    
    // Simulate magnetometer
    data.mag_x = 10.0f * cosf(sim_time / 5.0f);
    data.mag_y = 10.0f * sinf(sim_time / 7.0f);
    data.mag_z = 40.0f;
    
    return data;
}

DepthData PixhawkControl::read_depth() {
    DepthData data;
    
    // TODO: Replace with actual pressure sensor reading (BMP280, etc.)
    // For now: return realistic simulated depth data
    static float sim_depth = 0.0f;
    sim_depth += 0.01f;
    if (sim_depth > 100.0f) sim_depth = 0.0f;
    
    data.depth = sim_depth;
    data.pressure = 101325.0f + (sim_depth * 9806.65f);  // Pressure increases with depth
    data.temperature = 25.0f + 5.0f * sinf(sim_depth / 20.0f);
    
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

