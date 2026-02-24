#pragma once

#include "pixhawk_control.h"
#include "mavlink_handler.h"
#include <cstdint>

class MissionControl {
public:
    MissionControl();
    ~MissionControl();
    
    bool init(PixhawkControl* pixhawk);
    void handle_gui_command(const ControlPacket& cmd);
    void update_telemetry();
    TelemetryPacket get_telemetry();
    
    void arm_system();
    void disarm_system();
    
private:
    PixhawkControl* pixhawk_ptr;
    RobotState current_state;
    
    void update_state();
};
