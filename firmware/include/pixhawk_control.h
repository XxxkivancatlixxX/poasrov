#pragma once

#include <cstdint>

struct IMUData {
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;
    float mag_x, mag_y, mag_z;
};

struct DepthData {
    float depth;
    float pressure;
    float temperature;
};

class PixhawkControl {
public:
    PixhawkControl();
    ~PixhawkControl();
    
    bool init();
    void set_motor_throttle(uint8_t motor_id, float throttle);
    void set_all_motors(const float throttles[8]);
    IMUData read_imu();
    DepthData read_depth();
    void arm();
    void disarm();
    bool is_armed() const;
    float get_motor_throttle(uint8_t motor_id) const;
    
private:
    float motor_throttles[8];
    bool armed;
    void apply_motor_commands();
};
