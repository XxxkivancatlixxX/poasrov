#!/bin/bash

# Full QEMU Testing Setup
# Launches QEMU, telemetry bridge, and shows connection instructions
# This provides a complete testing environment without real hardware

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$PROJECT_DIR/firmware"
FIRMWARE_BIN="$FIRMWARE_DIR/build/src/firmware"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Check prerequisites
print_header "QEMU Full Test Setup"
echo ""

# 1. Check firmware
if [ ! -f "$FIRMWARE_BIN" ]; then
    print_error "Firmware not compiled: $FIRMWARE_BIN"
    echo ""
    echo "Build firmware:"
    echo "  cd $FIRMWARE_DIR/build"
    echo "  make"
    exit 1
fi
print_success "Firmware found: $(basename $FIRMWARE_BIN)"

# 2. Check QEMU
if ! command -v qemu-system-arm &> /dev/null; then
    print_error "QEMU not installed"
    echo ""
    echo "Install QEMU:"
    echo "  Arch: sudo pacman -S qemu-system-arm"
    echo "  Ubuntu: sudo apt-get install qemu-system-arm"
    exit 1
fi
print_success "QEMU installed"

# 3. Check Python
if ! command -v python3 &> /dev/null; then
    print_error "Python3 not found"
    exit 1
fi
print_success "Python3 found"

# 4. Check telemetry bridge
if [ ! -f "$PROJECT_DIR/telemetry_bridge.py" ]; then
    print_error "Telemetry bridge not found: $PROJECT_DIR/telemetry_bridge.py"
    exit 1
fi
print_success "Telemetry bridge found"

echo ""
print_header "Starting Components"
echo ""

# Launch QEMU in background (with pseudo-terminal for output)
print_info "Starting QEMU..."
mkfifo /tmp/qemu_output || true
qemu-system-arm \
    -machine stm32f4-discovery \
    -kernel "$FIRMWARE_BIN" \
    -nographic \
    -serial stdio \
    -monitor none \
    > /tmp/qemu_output 2>&1 &
QEMU_PID=$!
print_success "QEMU started (PID: $QEMU_PID)"

# Give QEMU time to boot
sleep 2

# Launch telemetry bridge in background
print_info "Starting telemetry bridge..."
python3 "$PROJECT_DIR/telemetry_bridge.py" --stdin 5760 > /tmp/bridge_output.log 2>&1 &
BRIDGE_PID=$!
print_success "Bridge started (PID: $BRIDGE_PID)"

# Give bridge time to start TCP server
sleep 1

# Check if bridge is listening
if netstat -ln 2>/dev/null | grep -q ":5760"; then
    print_success "Bridge listening on TCP:5760"
else
    print_error "Bridge failed to start TCP server"
    kill $QEMU_PID $BRIDGE_PID 2>/dev/null || true
    exit 1
fi

echo ""
print_header "✓ Testing Environment Ready"
echo ""
echo "QEMU is emulating Pixhawk firmware"
echo "Telemetry bridge is forwarding data to TCP:5760"
echo ""
print_info "To test:"
echo "  1. In this terminal: Monitor QEMU output"
echo "  2. In another terminal: Watch telemetry"
echo "     tail -f /tmp/bridge_output.log"
echo "  3. Run GUI and connect to localhost:5760"
echo ""
print_info "To stop:"
echo "  Press Ctrl+C here, or:"
echo "  kill $QEMU_PID $BRIDGE_PID"
echo ""

# Pipe QEMU output
echo "=== QEMU Output ==="
tail -f /tmp/qemu_output &
TAIL_PID=$!

# Trap to clean up on exit
cleanup() {
    echo ""
    print_info "Cleaning up..."
    kill $TAIL_PID 2>/dev/null || true
    kill $QEMU_PID 2>/dev/null || true
    kill $BRIDGE_PID 2>/dev/null || true
    rm -f /tmp/qemu_output /tmp/bridge_output.log
    print_success "Shutdown complete"
}

trap cleanup EXIT INT TERM

# Wait for all background processes
wait 2>/dev/null || true
