#include "mavlink_handler.h"
#include "pixhawk_control.h"
#include "mission_control.h"
#include "ethernet_comm.h"
#include <cstring>
#include <cmath>

static RobotState g_robot_state = {};
static ProtocolHandler protocol_handler;

void initialize_robot_state() {
    g_robot_state.armed = 0;
    g_robot_state.flight_mode = 0;
    
    g_robot_state.sensors.gyro_x = 0.0f;
    g_robot_state.sensors.gyro_y = 0.0f;
    g_robot_state.sensors.gyro_z = 0.0f;
    g_robot_state.sensors.accel_x = 0.0f;
    g_robot_state.sensors.accel_y = 0.0f;
    g_robot_state.sensors.accel_z = 9.81f;
    g_robot_state.sensors.mag_x = 0.0f;
    g_robot_state.sensors.mag_y = 0.0f;
    g_robot_state.sensors.mag_z = 0.0f;
    g_robot_state.sensors.depth = 0.0f;
    g_robot_state.sensors.temperature = 25.0f;
    g_robot_state.sensors.pressure = 101325.0f;
    
    g_robot_state.battery.voltage = 14.8f;
    g_robot_state.battery.current = 0.0f;
    g_robot_state.battery.capacity_mah = 18000.0f;
    g_robot_state.battery.percentage = 100;
    
    g_robot_state.camera.camera_type = 0;
    g_robot_state.camera.servo_min_pwm = 1100.0f;
    g_robot_state.camera.servo_max_pwm = 1900.0f;
    g_robot_state.camera.gimbal_type = 0;
    
    g_robot_state.water.pressure_offset = 0;
    g_robot_state.water.temp_offset = 0.0f;
    g_robot_state.water.salinity_type = 1;
    
    g_robot_state.pid_tuning.roll_p = 0.15f;
    g_robot_state.pid_tuning.roll_i = 0.02f;
    g_robot_state.pid_tuning.roll_d = 0.04f;
    g_robot_state.pid_tuning.pitch_p = 0.15f;
    g_robot_state.pid_tuning.pitch_i = 0.02f;
    g_robot_state.pid_tuning.pitch_d = 0.04f;
    g_robot_state.pid_tuning.yaw_p = 0.2f;
    g_robot_state.pid_tuning.yaw_i = 0.05f;
    g_robot_state.pid_tuning.yaw_d = 0.0f;
    g_robot_state.pid_tuning.depth_p = 2.5f;
    g_robot_state.pid_tuning.depth_i = 0.1f;
    g_robot_state.pid_tuning.depth_d = 0.5f;
    
    g_robot_state.roll = 0.0f;
    g_robot_state.pitch = 0.0f;
    g_robot_state.yaw = 0.0f;
}

void update_sensor_readings() {
    static float sim_depth = 0.0f;
    static float sim_temp = 25.0f;
    
    sim_depth += 0.01f;
    if (sim_depth > 100.0f) sim_depth = 0.0f;
    
    sim_temp = 25.0f + 5.0f * sinf(sim_depth / 20.0f);
    
    g_robot_state.sensors.depth = sim_depth;
    g_robot_state.sensors.temperature = sim_temp;
    g_robot_state.sensors.pressure = 101325.0f + (sim_depth * 9806.65f);
    
    g_robot_state.sensors.gyro_x = 0.5f * sinf(sim_depth / 10.0f);
    g_robot_state.sensors.gyro_y = 0.3f * cosf(sim_depth / 15.0f);
    g_robot_state.sensors.gyro_z = 0.2f * sinf(sim_depth / 20.0f);
    
    g_robot_state.sensors.accel_x = 0.2f * sinf(sim_depth / 8.0f);
    g_robot_state.sensors.accel_y = 0.15f * cosf(sim_depth / 12.0f);
    g_robot_state.sensors.accel_z = 9.81f + 0.1f * sinf(sim_depth / 10.0f);
    
    g_robot_state.sensors.mag_x = 10.0f * cosf(sim_depth / 5.0f);
    g_robot_state.sensors.mag_y = 10.0f * sinf(sim_depth / 7.0f);
    g_robot_state.sensors.mag_z = 40.0f;
    
    if (g_robot_state.armed) {
        g_robot_state.battery.current = 15.0f + 5.0f * sinf(sim_depth / 3.0f);
        g_robot_state.battery.voltage = 14.8f - (g_robot_state.battery.current * 0.01f);
    } else {
        g_robot_state.battery.current = 2.0f;
        g_robot_state.battery.voltage = 15.0f;
    }
    
    float capacity_used = 18000.0f * (1.0f - (g_robot_state.battery.voltage / 16.8f));
    g_robot_state.battery.percentage = (uint8_t)(((18000.0f - capacity_used) / 18000.0f) * 100.0f);
    
    g_robot_state.roll = 15.0f * sinf(sim_depth / 5.0f);
    g_robot_state.pitch = 10.0f * cosf(sim_depth / 7.0f);
    g_robot_state.yaw = sim_depth * 3.6f;
}

int main() {
    initialize_robot_state();
    protocol_handler.init();
    
    uint8_t rx_buffer[512];
    uint16_t rx_len = 0;
    
    while (1) {
        update_sensor_readings();
        
        TelemetryPacket telemetry = protocol_handler.create_telemetry_packet(g_robot_state);
        uint8_t tx_buffer[512];
        memcpy(tx_buffer, &telemetry, sizeof(telemetry));
        
        rx_len = 0;
        
        if (rx_len > 0) {
            ControlPacket* ctrl = (ControlPacket*)rx_buffer;
            if (ctrl->packet_type == 1) {
                g_robot_state.armed = ctrl->armed;
                g_robot_state.flight_mode = ctrl->flight_mode;
                
                for (uint8_t i = 0; i < ctrl->motor_count && i < 8; i++) {
                    if (g_robot_state.armed) {
                    }
                }
            }
        }
    }
    
    return 0;
}
