#pragma once

#include <cstdint>
#include <cstring>

struct RemoteSensorData {
    float gyro_x, gyro_y, gyro_z;
    float accel_x, accel_y, accel_z;
    float mag_x, mag_y, mag_z;
    float depth;
    float temperature;
    float pressure;
};

struct RemoteBatteryData {
    float voltage;
    float current;
    float capacity_mah;
    uint8_t percentage;
};

struct RemoteCameraData {
    uint8_t camera_type;
    float servo_min_pwm;
    float servo_max_pwm;
    uint8_t gimbal_type;
};

struct RemoteWaterSensorData {
    int32_t pressure_offset;
    float temp_offset;
    uint8_t salinity_type;
};

struct RemotePIDTuning {
    float roll_p, roll_i, roll_d;
    float pitch_p, pitch_i, pitch_d;
    float yaw_p, yaw_i, yaw_d;
    float depth_p, depth_i, depth_d;
};

struct RemoteRobotState {
    uint8_t armed;
    uint8_t flight_mode;
    RemoteSensorData sensors;
    RemoteBatteryData battery;
    RemoteCameraData camera;
    RemoteWaterSensorData water;
    RemotePIDTuning pid_tuning;
    float roll, pitch, yaw;
};

struct RemoteTelemetryPacket {
    uint8_t packet_type;
    RemoteRobotState state;
    uint8_t checksum;
};

class TelemetryReceiver {
public:
    TelemetryReceiver();
    ~TelemetryReceiver();
    
    void update_from_packet(const RemoteTelemetryPacket& pkt);
    const RemoteRobotState& get_state() const { return latest_state; }
    
private:
    RemoteRobotState latest_state;
};
