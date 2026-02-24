#pragma once

#include <cstdint>
#include <cstring>

struct MotorCommand {
    uint8_t motor_id;
    float throttle;
    uint8_t enabled;
};

struct SensorData {
    float gyro_x, gyro_y, gyro_z;
    float accel_x, accel_y, accel_z;
    float mag_x, mag_y, mag_z;
    float depth;
    float temperature;
    float pressure;
};

struct BatteryData {
    float voltage;
    float current;
    float capacity_mah;
    uint8_t percentage;
};

struct CameraData {
    uint8_t camera_type;
    float servo_min_pwm;
    float servo_max_pwm;
    uint8_t gimbal_type;
};

struct WaterSensorData {
    int32_t pressure_offset;
    float temp_offset;
    uint8_t salinity_type;
};

struct PIDTuning {
    float roll_p, roll_i, roll_d;
    float pitch_p, pitch_i, pitch_d;
    float yaw_p, yaw_i, yaw_d;
    float depth_p, depth_i, depth_d;
};

struct RobotState {
    uint8_t armed;
    uint8_t flight_mode;
    SensorData sensors;
    BatteryData battery;
    CameraData camera;
    WaterSensorData water;
    PIDTuning pid_tuning;
    float roll, pitch, yaw;
};

struct ControlPacket {
    uint8_t packet_type;
    uint8_t motor_count;
    MotorCommand motors[8];
    uint8_t armed;
    uint8_t flight_mode;
    uint8_t checksum;
};

struct TelemetryPacket {
    uint8_t packet_type;
    RobotState state;
    uint8_t checksum;
};

class ProtocolHandler {
public:
    ProtocolHandler();
    ~ProtocolHandler();
    
    bool init();
    void parse_control_packet(const uint8_t* data, uint16_t len);
    TelemetryPacket create_telemetry_packet(const RobotState& state);
    uint8_t calculate_checksum(const uint8_t* data, uint16_t len);
    
private:
    uint8_t sequence_counter;
};
