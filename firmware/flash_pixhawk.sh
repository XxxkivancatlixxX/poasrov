#!/bin/bash

# Pixhawk 2.4.8 Firmware Flashing Script
# Usage: ./flash_pixhawk.sh

set -e

FIRMWARE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$FIRMWARE_DIR/build/src"
FIRMWARE_BIN="firmware.bin"
FIRMWARE_ELF="$BUILD_DIR/firmware"

echo "===================================================="
echo "Pixhawk 2.4.8 Firmware Flashing Tool"
echo "===================================================="
echo ""

# Check if firmware binary exists
if [ ! -f "$FIRMWARE_ELF" ]; then
    echo "ERROR: Firmware binary not found: $FIRMWARE_ELF"
    echo "Build first: cd $FIRMWARE_DIR && mkdir -p build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake .. && make"
    exit 1
fi

echo "[1/5] Converting firmware to binary format..."
cd "$BUILD_DIR"
arm-none-eabi-objcopy -O binary firmware firmware.bin
echo "      Created: $BUILD_DIR/$FIRMWARE_BIN ($(stat -f%z firmware.bin 2>/dev/null || stat -c%s firmware.bin) bytes)"
echo ""

echo "[2/5] Checking DFU device connection..."
if ! dfu-util -l 2>/dev/null | grep -q "STM32"; then
    echo "ERROR: No STM32 device in DFU mode found!"
    echo ""
    echo "SOLUTION: Put Pixhawk in DFU mode:"
    echo "  1. Disconnect all power from Pixhawk"
    echo "  2. Connect USB cable (USB power only)"
    echo "  3. **HOLD Boot button down**"
    echo "  4. Apply power (via battery connector)"
    echo "  5. Release Boot button"
    echo "  6. LED should blink slowly = DFU mode"
    echo "  7. Run this script again"
    echo ""
    exit 1
fi

dfu-util -l
echo ""

echo "[3/5] Ready to flash. Confirm:"
echo "      Device: STM32F427VI (Pixhawk 2.4.8)"
echo "      File: $BUILD_DIR/$FIRMWARE_BIN"
echo "      Address: 0x08000000"
echo ""
read -p "Continue with flash? (y/n) " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 0
fi

echo "[4/5] FLASHING FIRMWARE..."
dfu-util -a 0 -D firmware.bin -s 0x08000000 -R

if [ $? -eq 0 ]; then
    echo ""
    echo "[5/5] Flash successful!"
    echo ""
    echo "Next steps:"
    echo "  1. Pixhawk should reset automatically"
    echo "  2. Wait 5 seconds for bootloader"
    echo "  3. Orange LED = ready"
    echo "  4. Check telemetry: nc -u 192.168.1.2 14550"
    echo ""
    echo "===================================================="
    echo "SUCCESS: Firmware flashed to Pixhawk 2.4.8"
    echo "===================================================="
else
    echo ""
    echo "ERROR: Flash failed!"
    echo ""
    echo "Troubleshooting:"
    echo "  - Check USB connection (try different cable)"
    echo "  - Ensure Pixhawk is in DFU mode (LED blinking slowly)"
    echo "  - Try: sudo dfu-util -a 0 -D firmware.bin -s 0x08000000 -R"
    echo ""
    exit 1
fi
