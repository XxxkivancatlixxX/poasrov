# Pixhawk ROV Firmware Backend

Custom firmware backend for ROV GUI application with direct motor control and telemetry streaming.

## Features

- Direct motor control (8 independent motors)
- ARM/DISARM system commands
- Real-time IMU and depth sensor telemetry
- Custom C++ protocol over Ethernet
- Integrated logging system
- 100Hz telemetry update rate

## Building

```bash
cd build
cmake ..
make
```

## Running

```bash
./firmware
```

## Protocol

### GUI to Firmware (Control Packet)
- Motor commands with individual throttle values (0.0 - 1.0)
- ARM/DISARM commands
- Motor enable/disable states

### Firmware to GUI (Telemetry Packet)
- Armed state
- Depth and temperature sensors
- IMU data (accelerometer, gyroscope, magnetometer)
- Motor throttle feedback

## Communication

- **IP**: 192.168.1.2
- **Port**: 14550
- **Protocol**: Custom binary (Ethernet UDP/TCP)
- **Update Rate**: 100Hz

## System Architecture

- `main.cpp`: Main firmware loop and packet routing
- `mission_control.cpp`: Mission planning and command execution
- `pixhawk_control.cpp`: Hardware abstraction for Pixhawk
- `ethernet_comm.cpp`: Network communication
- `mavlink_handler.cpp`: Custom protocol encoding/decoding

## Motor Mapping

Motors 0-7 mapped directly from GUI sliders M1-M8 with throttle values 0.0-1.0 converted to PWM 1100-1900.
