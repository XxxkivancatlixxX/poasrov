# Firmware Compilation and Deployment Guide

## 1. COMPILE THE FIRMWARE

```bash
cd /home/vujuvuju/rov/PCside/firmware
mkdir -p build
cd build
cmake ..
make -j4
```

This generates `firmware` executable in the `build` directory.

## 2. RUN THE FIRMWARE (on Pixhawk/Pi)

```bash
./firmware
```

Output will show:
```
=== ROV Firmware Backend ===
Initializing systems...
Ethernet initialized on 192.168.1.2:14550
Pixhawk initialized
Mission Control initialized
Protocol Handler initialized

Waiting for GUI connections...
```

## 3. FLASH TO PIXHAWK (if needed)

If you want to flash to actual Pixhawk hardware:

### Option A: Copy Binary
```bash
scp build/firmware user@pixhawk_ip:/home/user/rov/firmware
ssh user@pixhawk_ip
cd /home/user/rov
./firmware
```

### Option B: Cross-compile for ARM
Edit `CMakeLists.txt` to add ARM toolchain:
```cmake
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
```

## 4. CONTROLLER REMAPPING

### For GUI Motor Mapping:

Edit [src/ui.cpp](../src/ui.cpp) to customize motor-to-slider mapping:

```cpp
// Current mapping: M1-M8 sliders = Motor 0-7
// To remap, modify in ui_draw():
float slider_width = ImGui::GetContentRegionAvail().x / 2.0f - 5;
for (int i = 0; i < 8; ++i) {
    char label[32];
    snprintf(label, sizeof(label), "M%d", i + 1);
    // ...motor control code...
}
```

### For Controller Button Remapping:

Edit [PCside/input.cpp](../input.cpp):

```cpp
void input_update()
{
    if (!g_pad) return;

    g_state.axis_left_x  = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTX));
    g_state.axis_left_y  = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTY));
    
    // Remap buttons here:
    // SDL_CONTROLLER_BUTTON_A, B, X, Y, START, BACK
    // Assign to different functions as needed
}
```

### Motor Throttle Mapping (0.0-1.0 to PWM):

Edit [firmware/src/pixhawk_control.cpp](../firmware/src/pixhawk_control.cpp):

```cpp
void PixhawkControl::apply_motor_commands() {
    // Current: 1100-1900 PWM
    // throttle 0.0 = 1100 (stopped)
    // throttle 1.0 = 1900 (full speed)
    
    uint16_t pwm_value = 1100 + (int)(throttle * 800);
    
    // To customize:
    // uint16_t pwm_value = MIN_PWM + (int)(throttle * (MAX_PWM - MIN_PWM));
}
```

## 5. TROUBLESHOOTING

### Firmware won't compile:
```bash
cd build
cmake --clean-first .
make clean
cmake ..
make
```

### Can't connect to Pixhawk:
- Check IP: `ping 192.168.1.2`
- Check port: `netstat -tulpn | grep 14550`
- Verify Ethernet cable connection

### Motors not responding:
- Check ARM status in GUI (button must be pressed)
- Verify slider values in GUI are sending
- Check firmware console output for received packets

### Controller not working:
```bash
# List connected controllers
ls /dev/input/js*

# Test controller input
cat /dev/input/js0
```

## 6. QUICK START

### Terminal 1 - Run Firmware:
```bash
cd /home/vujuvuju/rov/PCside/firmware/build
./firmware
```

### Terminal 2 - Run GUI:
```bash
cd /home/vujuvuju/rov/PCside
./rov_gui
```

### Terminal 3 - Monitor Logs:
```bash
# Watch firmware output
tail -f firmware.log
```

Then:
1. Connect gamepad to PC
2. Launch GUI app
3. Press ARM in GUI
4. Move motor sliders
5. Watch firmware respond to commands
