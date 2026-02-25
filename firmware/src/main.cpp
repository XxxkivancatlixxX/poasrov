#include "mavlink_handler.h"
#include "pixhawk_control.h"
#include "mission_control.h"
#include "ethernet_comm.h"
#include "motor_config.h"
#include "hardware_hal.h"
#include <cstring>
#include <cmath>

static RobotState g_robot_state = {};
static ProtocolHandler protocol_handler;
static MotorConfigManager motor_config;

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

int main() {
    // Initialize hardware
    g_uart.init(57600);
    g_pwm.init();
    
    initialize_robot_state();
    protocol_handler.init();
    motor_config.init();
    
    uint8_t rx_buffer[512];
    uint16_t rx_len = 0;
    float motor_outputs[8] = {0};
    PixhawkControl pixhawk;
    pixhawk.init();
    
    while (1) {
        // Read actual sensor data from hardware
        IMUData imu = pixhawk.read_imu();
        DepthData depth = pixhawk.read_depth();
        
        g_robot_state.sensors.accel_x = imu.accel_x;
        g_robot_state.sensors.accel_y = imu.accel_y;
        g_robot_state.sensors.accel_z = imu.accel_z;
        g_robot_state.sensors.gyro_x = imu.gyro_x;
        g_robot_state.sensors.gyro_y = imu.gyro_y;
        g_robot_state.sensors.gyro_z = imu.gyro_z;
        g_robot_state.sensors.mag_x = imu.mag_x;
        g_robot_state.sensors.mag_y = imu.mag_y;
        g_robot_state.sensors.mag_z = imu.mag_z;
        g_robot_state.sensors.depth = depth.depth;
        g_robot_state.sensors.temperature = depth.temperature;
        g_robot_state.sensors.pressure = depth.pressure;
        
        TelemetryPacket telemetry = protocol_handler.create_telemetry_packet(g_robot_state);
        g_uart.write_bytes((uint8_t*)&telemetry, sizeof(telemetry));
        
        // Small delay to prevent UART buffer overflow (~10ms at 168MHz)
        for (volatile int i = 0; i < 100000; i++);
                if (g_uart.read_available()) {
            uint8_t byte = g_uart.read_byte();
            if (rx_len < 512) {
                rx_buffer[rx_len++] = byte;
            }
            
            if (rx_len >= sizeof(ControlPacket)) {
                ControlPacket* ctrl = (ControlPacket*)rx_buffer;
                if (ctrl->packet_type == 1) {
                    g_robot_state.armed = ctrl->armed;
                    g_robot_state.flight_mode = ctrl->flight_mode;
                    
                    if (g_robot_state.armed) {
                        // Check if direct motor commands are provided (motor test mode)
                        bool has_motor_commands = (ctrl->motor_count > 0);
                        
                        if (has_motor_commands) {
                            // Direct motor test mode - use provided throttle values
                            for (uint8_t i = 0; i < ctrl->motor_count && i < 8; i++) {
                                if (ctrl->motors[i].enabled) {
                                    float throttle = ctrl->motors[i].throttle;
                                    throttle = (throttle < 0.0f) ? 0.0f : (throttle > 1.0f) ? 1.0f : throttle;
                                    uint16_t pwm = (uint16_t)(1100.0f + (throttle * 800.0f));
                                    g_pwm.set_pwm(i, pwm);
                                } else {
                                    g_pwm.set_pwm(i, 1000);  // Disabled - neutral
                                }
                            }
                        } else {
                            // Normal flight mode - use roll/pitch/yaw/throttle from control packet
                            // For now, use roll/pitch/yaw from robot state and throttle from trigger_right (sent in control packet)
                            float roll = g_robot_state.roll * 0.017453f;
                            float pitch = g_robot_state.pitch * 0.017453f;
                            float yaw = g_robot_state.yaw * 0.017453f;
                            float throttle = ctrl->motors[0].throttle;  // Use first motor's throttle as overall throttle
                            
                            motor_config.calculate_motor_commands(roll, pitch, yaw, throttle, motor_outputs);
                            
                            for (uint8_t i = 0; i < 8; i++) {
                                uint16_t pwm = (uint16_t)(1100.0f + (motor_outputs[i] * 800.0f));
                                g_pwm.set_pwm(i, pwm);
                            }
                        }
                    } else {
                        for (uint8_t i = 0; i < 8; i++) {
                            g_pwm.set_pwm(i, 1000);
                        }
                    }
                }
                rx_len = 0;
            }
        }
    }
    
    return 0;
}
