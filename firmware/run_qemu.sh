#!/bin/bash

# QEMU STM32F4 Emulator for Pixhawk 2.4.8 Firmware Testing
# This emulates the Pixhawk 2.4.8 microcontroller for testing without hardware

set -e

FIRMWARE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_BIN="$FIRMWARE_DIR/build/src/firmware"
QEMU_LOG="$FIRMWARE_DIR/qemu.log"

echo "===================================================="
echo "QEMU STM32F4 Emulator - Pixhawk 2.4.8"
echo "===================================================="
echo ""

# Check if firmware exists
if [ ! -f "$FIRMWARE_BIN" ]; then
    echo "ERROR: Firmware binary not found: $FIRMWARE_BIN"
    echo ""
    echo "Build firmware first:"
    echo "  cd $FIRMWARE_DIR"
    echo "  mkdir -p build && cd build"
    echo "  cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake .."
    echo "  make"
    exit 1
fi

# Check if QEMU is installed
if ! command -v qemu-system-arm &> /dev/null; then
    echo "ERROR: qemu-system-arm not found!"
    echo ""
    echo "Install QEMU (Arch Linux):"
    echo "  sudo pacman -S qemu-system-arm"
    echo ""
    echo "Or Ubuntu/Debian:"
    echo "  sudo apt-get install qemu-system-arm"
    exit 1
fi

echo "[1/3] Checking firmware..."
file "$FIRMWARE_BIN"
ls -lh "$FIRMWARE_BIN"
echo ""

echo "[2/3] Starting QEMU emulation..."
echo "     Device: ARM Cortex-M4 (STM32F427VI)"
echo "     Firmware: $FIRMWARE_BIN"
echo "     Serial: stdio (this window)"
echo ""
echo "Output:"
echo "---"

# Run QEMU
qemu-system-arm \
    -machine stm32f4-discovery \
    -kernel "$FIRMWARE_BIN" \
    -nographic \
    -serial stdio \
    -monitor none \
    2>&1 | tee -a "$QEMU_LOG"

echo "---"
echo "[3/3] QEMU stopped"
echo ""
echo "Next steps:"
echo "  1. Verify telemetry output above"
echo "  2. In another terminal, run telemetry bridge:"
echo "     python3 telemetry_bridge.py"
echo "  3. Run GUI and connect to localhost:5760"
echo ""
