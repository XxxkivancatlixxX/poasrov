# Flash Firmware to Pixhawk 2.4.8

## Prerequisites

Install tools (Arch Linux):
```bash
sudo pacman -Syu
sudo pacman -S arm-none-eabi-gcc arm-none-eabi-binutils dfu-util
```

Or for Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y dfu-util arm-none-eabi-gcc arm-none-eabi-binutils
```

## Compile for Pixhawk 2.4.8 (STM32F427)

The firmware is already configured for STM32F427VI in `CMakeLists.txt`.

### Option 1: Using PlatformIO (Recommended)

Install PlatformIO:
```bash
pip install platformio
```

### Option 2: Manual ARM Cross-Compile

```bash
cd /home/vujuvuju/rov/PCside/firmware
rm -rf build
mkdir build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
make -j4
```

## Flash to Pixhawk

### Method 1: USB DFU Mode (Bootloader)

1. **Put Pixhawk in DFU Mode:**
   - Disconnect all connections
   - Hold BOOT button
   - Connect USB
   - Release BOOT button

2. **Verify DFU device:**
```bash
dfu-util -l
```

You should see:
```
Found DFU: [0483:df11] ver=0200, devnum=X, cfg=1, intf=0, alt=0, name="@Internal Flash  /0x08000000/04*016Kg,01*064Kg,07*128Kg", serial="..."
```

3. **Flash firmware:**
```bash
dfu-util -a 0 -s 0x08000000 -D build/firmware.elf
```

### Method 2: Serial Upload (via Mission Planner)

1. Open Mission Planner
2. Connect Pixhawk via USB
3. Go to **Initial Setup → Install Firmware**
4. Select your compiled firmware binary
5. Wait for upload to complete

### Method 3: Through QGroundControl

1. Install QGroundControl
2. Connect Pixhawk
3. **Vehicle Setup → Firmware**
4. Choose custom firmware file
5. Flash

## Build as Binary for Pixhawk

Create ARM toolchain file:

```bash
cat > arm-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE INTERNAL "")

set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections" CACHE INTERNAL "")
EOF
```

Then compile:
```bash
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
make
arm-none-eabi-objcopy -O binary firmware firmware.bin
```

## Verify Flash

After flashing, restart Pixhawk and check:

```bash
# Connect via serial (e.g., /dev/ttyUSB0)
minicom -D /dev/ttyUSB0 -b 115200

# You should see startup messages
```

## Rollback

If something goes wrong, restore original ArduSub:
```bash
# Download official ArduSub firmware
wget https://firmware.ardupilot.org/Sub/stable/pixhawk1/ardusub.elf

# Flash back
dfu-util -a 0 -s 0x08000000 -D ardusub.elf
```

## Troubleshooting

**"dfu-util: error opening USB device" or "Permission denied":**
```bash
sudo usermod -a -G dialout $USER
sudo usermod -a -G plugdev $USER
# Logout and login
```

**Pixhawk won't enter DFU mode:**
- Try holding BOOT longer (5 seconds)
- Check USB cable is working
- Try different USB port

**Flash fails mid-upload:**
- Re-enter DFU mode
- Try flashing again
- Check USB power supply is adequate

**Pixhawk doesn't boot after flash:**
- Check firmware is correct for STM32F427
- Restore from backup
- Contact support with error message
