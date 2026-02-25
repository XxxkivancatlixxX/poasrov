#!/bin/bash

# QEMU Testing Checklist
# Run this to verify all components are ready before testing

echo "========================================="
echo "QEMU Test Environment Checker"
echo "========================================="
echo ""

PASS=0
FAIL=0

check_command() {
    if command -v "$1" &> /dev/null; then
        echo "✓ $1: $(which $1)"
        ((PASS++))
    else
        echo "✗ $1: NOT FOUND"
        ((FAIL++))
    fi
}

check_file() {
    if [ -f "$1" ]; then
        echo "✓ $1: $(ls -lh $1 | awk '{print $5, $9}')"
        ((PASS++))
    else
        echo "✗ $1: NOT FOUND"
        ((FAIL++))
    fi
}

check_command_with_hint() {
    if command -v "$1" &> /dev/null; then
        echo "✓ $1"
        ((PASS++))
    else
        echo "✗ $1: $2"
        ((FAIL++))
    fi
}

echo "System Tools"
echo "-----------"
check_command "qemu-system-arm"
check_command "python3"
check_command "arm-none-eabi-gcc"
echo ""

echo "Project Files"
echo "-----------"
check_file "/home/vujuvuju/rov/PCside/firmware/build/src/firmware"
check_file "/home/vujuvuju/rov/PCside/telemetry_bridge.py"
check_file "/home/vujuvuju/rov/PCside/firmware/run_qemu.sh"
check_file "/home/vujuvuju/rov/PCside/test_qemu_full.sh"
check_file "/home/vujuvuju/rov/PCside/QEMU_QUICK_START.md"
echo ""

echo "Scripts Executable"
echo "-----------"
if [ -x "/home/vujuvuju/rov/PCside/firmware/run_qemu.sh" ]; then
    echo "✓ run_qemu.sh is executable"
    ((PASS++))
else
    echo "✗ run_qemu.sh is NOT executable"
    ((FAIL++))
fi

if [ -x "/home/vujuvuju/rov/PCside/test_qemu_full.sh" ]; then
    echo "✓ test_qemu_full.sh is executable"
    ((PASS++))
else
    echo "✗ test_qemu_full.sh is NOT executable"
    ((FAIL++))
fi

if [ -x "/home/vujuvuju/rov/PCside/telemetry_bridge.py" ]; then
    echo "✓ telemetry_bridge.py is executable"
    ((PASS++))
else
    echo "✗ telemetry_bridge.py is NOT executable"
    echo "   Run: chmod +x /home/vujuvuju/rov/PCside/telemetry_bridge.py"
    ((FAIL++))
fi
echo ""

echo "Python Dependencies"
echo "-----------"
if python3 -c "import socket" 2>/dev/null; then
    echo "✓ socket module available"
    ((PASS++))
else
    echo "✗ socket module missing (should be builtin)"
    ((FAIL++))
fi

if python3 -c "import threading" 2>/dev/null; then
    echo "✓ threading module available"
    ((PASS++))
else
    echo "✗ threading module missing (should be builtin)"
    ((FAIL++))
fi

# Optional serial module
if python3 -c "import serial" 2>/dev/null; then
    echo "✓ pyserial available (for real hardware)"
    ((PASS++))
else
    echo "⚠ pyserial NOT available (OK for QEMU, needed for real hardware)"
    echo "   Install: pip3 install pyserial"
fi
echo ""

echo "Port Availability"
echo "-----------"
if command -v netstat &> /dev/null; then
    if netstat -ln 2>/dev/null | grep -q ":5760"; then
        echo "⚠ Port 5760 is in use!"
        echo "   Kill with: pkill -f telemetry_bridge"
        ((FAIL++))
    else
        echo "✓ Port 5760 available"
        ((PASS++))
    fi
elif command -v ss &> /dev/null; then
    if ss -ln 2>/dev/null | grep -q ":5760"; then
        echo "⚠ Port 5760 is in use!"
        ((FAIL++))
    else
        echo "✓ Port 5760 available"
        ((PASS++))
    fi
else
    echo "⚠ netstat/ss not found, skipping port check"
fi
echo ""

echo "Optional: Compiled GUI"
echo "-----------"
if [ -f "/home/vujuvuju/rov/PCside/gui" ]; then
    echo "✓ GUI binary found"
    ((PASS++))
else
    echo "⚠ GUI binary not found"
    echo "   Build with: cd /home/vujuvuju/rov/PCside && make -f Makefile.gui"
fi
echo ""

echo "========================================="
echo "Results: $PASS passed, $FAIL failed"
echo "========================================="
echo ""

if [ $FAIL -eq 0 ]; then
    echo "✓ All checks passed! Ready to test."
    echo ""
    echo "Start testing:"
    echo "  ./test_qemu_full.sh"
    echo ""
    echo "Or manual mode:"
    echo "  Terminal 1: cd firmware && ./run_qemu.sh"
    echo "  Terminal 2: python3 telemetry_bridge.py --stdin 5760"
    echo "  Terminal 3: ./gui localhost 5760"
    exit 0
else
    echo "✗ Fix the above issues before testing"
    exit 1
fi
