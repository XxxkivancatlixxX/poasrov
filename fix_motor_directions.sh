#!/bin/bash
# Quick script to reverse motors 3 and 4 via MAVLink
# This fixes the forward/backward thrust direction

echo "This script will reverse motors 3 and 4 in ArduSub"
echo "Make sure your ROV is connected first!"
echo ""
echo "Run this from the QML console or add buttons to call:"
echo "  backend.reverseMotor(3)"
echo "  backend.reverseMotor(4)"
echo ""
echo "Or connect via MAVProxy and run:"
echo "  param set MOT_3_DIRECTION -1"
echo "  param set MOT_4_DIRECTION -1"
