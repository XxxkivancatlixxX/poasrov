#pragma once

#include <cstdint>
#include <cstring>

// Mirror firmware structures for PC side telemetry parsing
struct TelemetrySensorData {
    float gyro_x, gyro_y, gyro_z;
    float accel_x, accel_y, accel_z;
    float mag_x, mag_y, mag_z;
    float depth;
    float temperature;
    float pressure;
};

struct TelemetryBatteryData {
    float voltage;
    float current;
    float capacity_mah;
    uint8_t percentage;
};

struct TelemetryCameraData {
    uint8_t camera_type;
    float servo_min_pwm;
    float servo_max_pwm;
    uint8_t gimbal_type;
};

struct TelemetryWaterSensorData {
    int32_t pressure_offset;
    float temp_offset;
    uint8_t salinity_type;
};

struct TelemetryPIDTuning {
    float roll_p, roll_i, roll_d;
    float pitch_p, pitch_i, pitch_d;
    float yaw_p, yaw_i, yaw_d;
    float depth_p, depth_i, depth_d;
};

struct TelemetryRobotState {
    uint8_t armed;
    uint8_t flight_mode;
    TelemetrySensorData sensors;
    TelemetryBatteryData battery;
    TelemetryCameraData camera;
    TelemetryWaterSensorData water;
    TelemetryPIDTuning pid_tuning;
    float roll, pitch, yaw;
};

struct TelemetryPacket {
    uint8_t packet_type;
    TelemetryRobotState state;
    uint8_t checksum;
};

class TelemetryParser {
public:
    TelemetryParser();
    
    bool parse_packet(const uint8_t* data, uint16_t len, TelemetryPacket& packet);
    uint8_t calculate_checksum(const uint8_t* data, uint16_t len);
};
