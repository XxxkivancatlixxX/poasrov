# QEMU Testing Guide - Pixhawk 2.4.8 Emulation

## Quick Start

### Terminal 1: Run QEMU with firmware
```bash
cd /home/vujuvuju/rov/PCside/firmware
./run_qemu.sh
```

Expected output: Binary telemetry packets flowing to stdout

### Terminal 2: Run telemetry bridge
```bash
cd /home/vujuvuju/rov/PCside
python3 telemetry_bridge.py 5760
```

This bridges QEMU serial output to TCP:5760

### Terminal 3: Run GUI
```bash
# Build GUI if needed
cd /home/vujuvuju/rov/PCside
make -f Makefile.gui

# Run GUI - connect to localhost:5760 instead of 192.168.1.2:5760
./gui localhost 5760
```

## What's Being Tested

**QEMU emulates:**
- ARM Cortex-M4 CPU (STM32F427VI)
- Timer modules (for PWM generation)
- UART serial communication
- Memory (Flash + RAM)

**Firmware does:**
- Initializes UART at 57600 baud
- Initializes PWM timers (TIM1, TIM3)
- Generates fake telemetry every loop iteration
- Sends binary TelemetryPacket over serial
- Receives control commands (not yet tested)

**Bridge does:**
- Reads serial bytes from QEMU
- Assembles into packets
- Forwards to TCP clients

**GUI does:**
- Connects to bridge TCP socket
- Parses telemetry packets
- Displays all sensor data
- Sends motor commands (received by firmware)

## Troubleshooting

### QEMU won't start
```bash
# Check if installed
which qemu-system-arm

# Install (Arch)
sudo pacman -S qemu-system-arm

# Install (Ubuntu/Debian)
sudo apt-get install qemu-system-arm
```

### No telemetry data visible
- Check firmware compiled: `ls -lh firmware/build/src/firmware`
- Rebuild: `cd firmware/build && make`
- Check QEMU output for errors

### Bridge won't connect to QEMU serial
- Make sure QEMU is running in first terminal
- Check TCP port 5760 is free: `netstat -ln | grep 5760`
- Try different port: `python3 telemetry_bridge.py 5761`

### GUI won't connect
- Verify bridge is running: `netstat -ln | grep 5760`
- Check GUI connection address (localhost, not 192.168.1.2)
- Confirm telemetry data in QEMU terminal

## Testing Motor Commands

Once GUI connects:
1. Go to "Motor Config" tab
2. Select "M1" motor
3. Move slider to 0.5 (50%)
4. Watch QEMU output for PWM values sent to TIM1 CCR1

Expected: PWM values changing from 1000-2000 microseconds

## What's NOT Working Yet

- Real IMU reading (using fake data)
- Real motor control (PWM pins don't actually exist in QEMU)
- GPS/compass integration
- Depth sensor reading
- Battery ADC reading

## Next Steps

1. ✅ Verify telemetry flows QEMU → Bridge → GUI
2. ✅ Verify motor mixing math in firmware
3. Test controller input mapping
4. Implement real sensor reading
5. Flash to real Pixhawk 2.4.8

## Debug With GDB

Run QEMU with GDB server:
```bash
qemu-system-arm \
    -machine stm32f4-discovery \
    -kernel firmware/build/src/firmware \
    -nographic \
    -serial stdio \
    -s -S &

# In another terminal
arm-none-eabi-gdb firmware/build/src/firmware
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```
