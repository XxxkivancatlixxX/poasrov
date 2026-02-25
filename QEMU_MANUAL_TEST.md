# QEMU Testing - Manual Step-by-Step Guide

## Overview

Test the Pixhawk firmware in QEMU emulation without needing real hardware. The flow is:

```
QEMU (emulates Pixhawk) 
  ↓ (serial output)
Telemetry Bridge (TCP server on 5760)
  ↓ (TCP packets)
GUI Application (displays telemetry & sends commands)
```

## Prerequisites

### 1. Install QEMU
```bash
# Arch Linux
sudo pacman -S qemu-system-arm

# Ubuntu/Debian
sudo apt-get install qemu-system-arm

# Verify
qemu-system-arm --version
```

### 2. Install Python Dependencies (optional)
```bash
# Only needed if using serial connection on real hardware
pip3 install pyserial
```

### 3. Build Firmware
```bash
cd /home/vujuvuju/rov/PCside/firmware
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
make
# Verify: ls -lh src/firmware
```

## Testing Options

### Option A: Fully Automated (Easiest)
```bash
cd /home/vujuvuju/rov/PCside
./test_qemu_full.sh
```

This launches:
1. QEMU with firmware
2. Telemetry bridge
3. Shows live output

Then run GUI in another terminal and connect to `localhost:5760`.

### Option B: Manual (Three Terminals)

**Terminal 1: Run QEMU**
```bash
cd /home/vujuvuju/rov/PCside/firmware
./run_qemu.sh
```

Expected output:
```
==================================================
QEMU STM32F4 Emulator - Pixhawk 2.4.8
==================================================

[1/3] Checking firmware...
...
[2/3] Starting QEMU emulation...
Output:
---
<telemetry packets output>
```

This window shows raw serial output from QEMU.

**Terminal 2: Run Telemetry Bridge**
```bash
# Method 1: Read from QEMU's serial output
python3 /home/vujuvuju/rov/PCside/telemetry_bridge.py --stdin 5760

# Method 2: If redirecting QEMU to a named pipe
python3 /home/vujuvuju/rov/PCside/telemetry_bridge.py /tmp/qemu_serial 5760
```

Expected output:
```
=== Telemetry Bridge ===
Input:  STDIN (QEMU)
Output: TCP 0.0.0.0:5760
Status: Starting...

[HH:MM:SS] TCP server listening on port 5760
[HH:MM:SS] Packets: 10 | Clients: 0
[HH:MM:SS] Packets: 20 | Clients: 0
```

**Terminal 3: Run GUI**
```bash
cd /home/vujuvuju/rov/PCside
# Build if needed:
# make -f Makefile.gui

# Run and connect to localhost:5760
./gui localhost 5760
```

Watch the GUI:
- All sensor values should be animating (depth, temp, gyro, etc.)
- Battery voltage around 12V
- All 8 motors should show values 1000-2000µs

### Option C: Direct QEMU Testing (No Bridge)
```bash
cd /home/vujuvuju/rov/PCside/firmware
qemu-system-arm \
    -machine stm32f4-discovery \
    -kernel build/src/firmware \
    -nographic \
    -serial stdio
```

This directly shows telemetry packets as binary output (hard to read, but confirms firmware is running).

## What to Look For

### QEMU Output
You should see no error messages, just:
- Periodic initialization messages
- Binary telemetry data being written to stdout (may show as garbage)

### Bridge Output
```
[14:23:45] Packets: 10 | Clients: 0
[14:23:46] Packets: 20 | Clients: 0
[14:23:47] Packets: 30 | Clients: 1    # <-- GUI connected!
```

Look for:
- TCP server listening on :5760
- Packet count increasing (20-50 per second typical)
- Client count = 1 when GUI connects

### GUI Output
Once connected to bridge, you should see:
- **Flight Tab**: Animated depth meter, temperature gauge
- **Sensors Tab**: Gyroscope (X/Y/Z) values bouncing around
- **Battery Tab**: Voltage ~12V, current ~2A, capacity ~90%
- **Motor Config Tab**: Motor angles showing, reversal flags visible
- **Motor commands**: Moving any motor slider should broadcast to QEMU

## Troubleshooting

### "QEMU not found"
```bash
which qemu-system-arm
# If not found, install (see Prerequisites section)
```

### "Bridge won't start" or "Address already in use"
```bash
# Check if something is on port 5760
lsof -i :5760
netstat -ln | grep 5760

# Use different port
python3 telemetry_bridge.py --stdin 5761
```

### "No telemetry data visible"
1. Check QEMU is outputting (Terminal 1 shows output)
2. Check bridge is reading (Terminal 2 shows "Packets: X")
3. Check TCP connection (Bridge should show "Clients: 1" when GUI connects)

### "GUI won't connect"
1. Verify bridge is running: `netstat -ln | grep 5760`
2. Use `localhost` not `127.0.0.1` (may be same, but localhost is clearer)
3. Try connecting manually: `echo "test" | nc localhost 5760`
4. Check GUI console for error messages

### "Firmware crashes in QEMU"
1. This is expected! The firmware tries to access real hardware (timers, I/O pins) that don't exist in QEMU
2. QEMU still successfully emulates enough to test serial I/O and telemetry
3. This is why motor spin tests require real hardware

## Testing Motor Commands

Once GUI is connected:

1. **Motor Config Tab**:
   - Change motor 1 reversal: watch QEMU output (PWM value should reverse)
   - Change motor 1 angle from 0° to 45°: Motor mixing matrix recalculates

2. **Flight Tab**:
   - Move Motor 1 slider: Firmware receives command, calculates motor thrust
   - Watch all 8 motor values update in Motor Config tab

3. **Verify Motor Mixing**:
   - Roll the plane: Motors 1/3/5/7 go up, 2/4/6/8 go down
   - Pitch: Motors 1/2/7/8 up, 3/4/5/6 down
   - Yaw: Alternating motors increase/decrease

## Performance Metrics

Expected telemetry rate:
- QEMU: ~50-100 telemetry packets/sec (depending on host CPU)
- Bridge: Should relay all packets without loss
- GUI: Should display 30-60 FPS with live telemetry

Network traffic:
- Each packet: ~512 bytes
- Rate: 50 packets/sec = 25 KB/sec typical
- Peak: ~100 packets/sec = 50 KB/sec (still well below TCP limits)

## Next Steps After QEMU Testing

1. **Verify all tabs work**: Flight, Motors, Config, Controller mapping
2. **Test controller input mapping**: Connect gamepad, move sticks
3. **Log telemetry data**: Record 30 seconds of data for analysis
4. **Real hardware prep**:
   - Get Pixhawk 2.4.8 and ESCs
   - Connect Pi 5 to Pixhawk UART
   - Flash firmware using `flash_pixhawk.sh`
   - Repeat this test with real hardware

## Useful Commands

```bash
# Check if QEMU is running
pgrep -a qemu-system-arm

# Kill QEMU gracefully
pkill -f qemu-system-arm

# Monitor bridge in real-time
tail -f /tmp/bridge_output.log

# Check TCP connections
netstat -an | grep 5760
lsof -i :5760

# View QEMU log
tail -f firmware/qemu.log

# Clean up QEMU temp files
rm -f /tmp/qemu_* /tmp/bridge_output.log

# Run with GDB debugging (advanced)
qemu-system-arm \
    -machine stm32f4-discovery \
    -kernel firmware/build/src/firmware \
    -nographic \
    -serial stdio \
    -s -S &
arm-none-eabi-gdb firmware/build/src/firmware
(gdb) target remote :1234
```

## Expected Telemetry Packet Structure

The firmware sends ~512 byte packets containing:
- Sensor data: gyro, accel, magnetometer, depth, temperature
- Battery: voltage, current, capacity percentage
- Motor state: PWM values for all 8 motors
- Config: PID tuning parameters, motor reversal flags
- Status: armed/disarmed, frame type

The bridge relays these to GUI clients as-is.
