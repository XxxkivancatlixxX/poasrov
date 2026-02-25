# Flashing Firmware to Pixhawk 2.4.8

## Prerequisites
- dfu-util installed: `pacman -S dfu-util` (Arch Linux)
- Pixhawk 2.4.8 connected via USB
- Built firmware binary: `build/src/firmware`

## Flash via DFU Mode

### Step 1: Put Pixhawk in DFU Mode
1. Disconnect power from Pixhawk
2. Connect USB cable to Pixhawk (still no power)
3. **While holding the Boot button**, apply power (via USB or battery)
4. Release Boot button
5. LED should blink slowly = DFU mode active

### Step 2: Run Flash Script
```bash
cd /home/vujuvuju/rov/PCside/firmware
./flash_pixhawk.sh
```

### Step 3: Reset Pixhawk
- Power cycle the board
- Wait 5 seconds for bootloader to run
- Firmware will start automatically

## Manual DFU Flash Command
```bash
# Convert to binary format
arm-none-eabi-objcopy -O binary build/src/firmware firmware.bin

# List connected DFU devices
dfu-util -l

# Flash to Pixhawk (STM32F427VI @ 0x08000000)
dfu-util -a 0 -D firmware.bin -s 0x08000000 -R

# -a 0 = interface 0
# -D = download to device
# -s = start address
# -R = reset after flash
```

## Verify Flash Success
- LED should blink 3 times after reset = successful boot
- Orange LED steady = ready
- Red LED = error

## If Flash Fails

### Check USB Connection
```bash
lsusb | grep STM
```
Should show: `STM Microelectronics STM Device in DFU Mode`

### Check DFU Device
```bash
dfu-util -l
```
Should list `STM32 STM32F4xx`

### Reset Pixhawk if Stuck
1. Disconnect USB
2. Wait 10 seconds
3. Reconnect USB
4. Should return to normal boot mode

## Troubleshooting

### "dfu-util: Cannot open DFU device"
- Pixhawk not in DFU mode - repeat Step 1
- USB cable faulty - try different cable
- Wrong permissions - use `sudo`

### "Timeout writing to DFU device"
- Power supply issue - ensure 5V stable
- USB hub problem - connect directly to PC
- Try: `dfu-util -D firmware.bin -a 0 -s 0x08000000:leave`

### Firmware won't boot after flash
- Incorrect start address (use 0x08000000 for user firmware)
- Bootloader corrupted - needs STM32 Cube programmer
- Flash to address 0x08004000 instead (alternate location)

## What the Firmware Does
- Motor mixing and control (8 motors configurable)
- Ethernet telemetry at 192.168.1.2:14550
- Sensor simulation (gyro, accel, depth, temp)
- Battery monitoring
- Flight modes and arming system
