# Controller Settings Guide

## Overview

The ROV control application now includes a comprehensive controller configuration system that allows you to customize how your gamepad/joystick controls individual motors.

## Features

### Individual Motor Control
- Configure each of the 8 motors independently
- Assign multiple controller inputs to a single motor
- Combine inputs (e.g., left stick + right stick for complex movements)

### Flexible Input Mapping
- Map any controller axis to any motor:
  - Left Stick X/Y
  - Right Stick X/Y
  - Left Trigger
  - Right Trigger
- Set custom scale factors for each input
- Invert inputs as needed

### Default Configuration
The system comes with a QGC-compatible default profile:
- **Motors 1-4 (Horizontal)**: Vectored thruster configuration
  - Forward/backward: Left Stick Y
  - Strafe left/right: Left Stick X
  - Yaw rotation: Right Stick X
- **Motors 5-8 (Vertical)**: Up/down control
  - Ascend: Right Trigger
  - Descend: Left Trigger

## How to Use

### Accessing Controller Settings
1. Click the "PoasControll" button in the top toolbar
2. Navigate to the "Controller" tab in the settings dialog

### Configuring Motors

#### Example: Left Joystick Controls Motors 1, 2, and 3

1. **Select Motor 1**
   - Click the "Select Motor" dropdown
   - Choose "Motor 1"

2. **Clear Existing Mappings** (optional)
   - Click "Clear Mappings" to start fresh

3. **Add Custom Mapping**
   - Click "Add Custom Mapping"
   - Select "Left Stick Y" from Input Source
   - Set Scale to 1.00 (or adjust as needed)
   - Check "Invert Input" if needed
   - Click "Add"

4. **Repeat for Motors 2 and 3**
   - Select each motor and add the same mapping
   - Adjust scale/invert settings per motor if needed

#### Example: Complex Motor Mix

To create a motor that responds to both forward and strafe:

1. Select the motor
2. Add mapping: Left Stick Y, Scale 1.0
3. Add mapping: Left Stick X, Scale 0.5
4. The motor will now respond to both inputs combined

### Settings

- **Number of Motors**: Set how many motors your ROV has (1-8)
- **Deadzone**: Adjust controller deadzone (0-30%)
- **Max Throttle**: Safety limit in Device Settings tab

### Profiles

- **QGC Default (8 Motors)**: Standard QGroundControl-compatible configuration
- **Reset to Default**: Restore the default QGC profile

## Technical Details

### Motor Output Calculation
- Each motor starts at neutral (50% = 1500 PWM)
- Input contributions are accumulated
- Final output is clamped to 0-100% range
- Max throttle safety limit is applied

### Input Processing
1. Raw controller input (-1.0 to 1.0)
2. Deadzone applied
3. Expo curve applied (if configured)
4. Scale factor applied
5. Inversion applied (if enabled)
6. Contributions summed per motor
7. Converted to PWM range (1100-1900)

## Tips

- Start with the default profile and modify incrementally
- Test each motor individually using the Motor Testing feature
- Use lower scale values (0.3-0.7) for fine control
- Combine multiple inputs for complex movements
- Save your configuration before experimenting

## Troubleshooting

**Motors not responding:**
- Check that joystick control is enabled in Device Settings
- Verify the vehicle is armed
- Check max throttle setting isn't too low

**Unexpected motor behavior:**
- Review motor mappings for conflicts
- Check invert settings
- Verify scale factors are reasonable
- Reset to default profile and test

**Controller not detected:**
- Ensure SDL2 is installed
- Check controller is connected before starting app
- Try unplugging and reconnecting controller
