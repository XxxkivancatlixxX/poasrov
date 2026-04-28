# Controller Settings Update Summary

## What Was Added

A complete controller configuration system that allows individual motor control mapping with a user-friendly GUI.

## New Files Created

1. **qml/ControllerSettingsPage.qml** - New settings page for controller configuration
2. **CONTROLLER_SETTINGS.md** - User documentation
3. **CONTROLLER_UPDATE_SUMMARY.md** - This file

## Modified Files

### Core Controller System
- **controller_config.h** - Redesigned for motor-based mapping
  - Added `MotorMapping` struct for individual motor control
  - Added `AxisInput` enum for specific axis identification
  - Added `calculate_motor_outputs()` for flexible motor mixing
  - Default QGC profile with 8-motor vectored configuration

- **controller_config.cpp** - Implemented motor mapping logic
  - Motor contribution accumulation system
  - Deadzone and expo curve support
  - Default QGC profile matches standard ROV configuration

- **joystick_control.h/cpp** - Integrated with new config system
  - Uses `ControllerConfigManager` for all motor calculations
  - Removed hardcoded motor mixing
  - Maintains safety limits and throttle capping

### Backend Integration
- **Backend.h** - Added controller configuration methods:
  - `getControllerProfileName()` - Get active profile name
  - `getNumMotors()` / `setNumMotors()` - Motor count configuration
  - `addMotorMapping()` - Add custom motor mappings
  - `clearMotorMappings()` - Clear motor mappings
  - `resetToDefaultProfile()` - Restore defaults
  - `getMotorMappings()` - Query current mappings
  - `setJoystickDeadzone()` - Adjust deadzone

- **Backend.cpp** - Implemented all new methods with QML integration

### UI Updates
- **qml/Main.qml** - Added "Controller" tab to settings dialog
- **Makefile** - Added controller_config.cpp to build

## Key Features

### 1. Individual Motor Control
Users can now say: "I want left joystick to control motors 1, 2, and 3"
- Select any motor (1-8)
- Add multiple input sources per motor
- Each input has independent scale and invert settings

### 2. Flexible Input Mapping
- 6 input sources: Left/Right Stick X/Y, Left/Right Triggers
- Scale factors: -1.0 to 1.0 (adjustable in 0.01 increments)
- Invert option for each mapping
- Multiple inputs combine additively

### 3. Default QGC Profile
Standard 8-motor ROV configuration:
- Motors 1-4: Horizontal (forward/strafe/yaw)
- Motors 5-8: Vertical (up/down)
- Matches QGroundControl behavior

### 4. User-Friendly GUI
- Visual motor selector
- Current mappings display
- One-click reset to defaults
- Custom mapping dialog with validation

## Example Use Cases

### Simple: Single Axis Control
"Make motor 1 follow left stick Y"
1. Select Motor 1
2. Add mapping: Left Stick Y, scale 1.0

### Advanced: Combined Control
"Make motor 1 respond to both forward and strafe"
1. Select Motor 1
2. Add mapping: Left Stick Y, scale 1.0
3. Add mapping: Left Stick X, scale 0.5

### Custom: Inverted Control
"Make motor 2 go opposite of left stick"
1. Select Motor 2
2. Add mapping: Left Stick Y, scale 1.0, inverted

## Technical Implementation

### Motor Output Calculation
```
For each motor:
  1. Start at neutral (0.5)
  2. For each mapping:
     - Get input value
     - Apply deadzone
     - Apply expo
     - Apply scale and invert
     - Accumulate contribution
  3. Convert to 0-1 range
  4. Apply max throttle limit
  5. Send to ROV
```

### Data Flow
```
Controller Input → SDL2 → ControllerState
                           ↓
                    JoystickControl
                           ↓
                  ControllerConfigManager
                           ↓
                  calculate_motor_outputs()
                           ↓
                    ROV::setMotorThrottles()
                           ↓
                    MAVLink RC_CHANNELS_OVERRIDE
```

## Testing Recommendations

1. **Verify Default Profile**
   - Start app, connect controller
   - Check all 8 motors respond correctly
   - Test forward/strafe/yaw/vertical

2. **Test Custom Mapping**
   - Clear motor 1 mappings
   - Add Left Stick X only
   - Verify motor 1 responds to strafe only

3. **Test Multiple Inputs**
   - Add both Left Stick X and Y to motor 1
   - Verify combined response

4. **Test UI**
   - Open Controller settings
   - Verify current mappings display
   - Test reset to default

## Future Enhancements

Potential additions:
- Save/load custom profiles to file
- Profile presets (racing, precision, etc.)
- Button mapping support
- Curve editor for expo
- Real-time motor output visualization
- Profile sharing/import/export

## Compatibility

- Maintains backward compatibility with existing code
- Default profile matches previous hardcoded behavior
- No changes required to existing ROV/MAVLink code
- QML-only UI additions (no C++ UI dependencies)
