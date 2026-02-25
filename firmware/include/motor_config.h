#pragma once

#include <cstdint>
#include <cstring>

#define MAX_MOTORS 8
#define MAX_FRAMES 10
#define MAX_MOTOR_CONFIGS 5

enum FrameType {
    FRAME_VECTORED = 0,
    FRAME_QUADCOPTER = 1,
    FRAME_HEXACOPTER = 2,
    FRAME_OCTOCOPTER = 3,
    FRAME_CUSTOM = 4
};

struct MotorConfig {
    uint8_t motor_id;
    float angle_deg;
    uint8_t reversed;
    float thrust_factor;
};

struct FrameConfig {
    FrameType frame_type;
    uint8_t num_motors;
    MotorConfig motors[MAX_MOTORS];
    char name[32];
};

struct MotorMixing {
    float roll_mix[MAX_MOTORS];
    float pitch_mix[MAX_MOTORS];
    float yaw_mix[MAX_MOTORS];
    float throttle_mix[MAX_MOTORS];
};

class MotorConfigManager {
public:
    MotorConfigManager();
    ~MotorConfigManager();
    
    bool init();
    bool set_frame(FrameType frame);
    bool load_config(const char* config_file);
    bool save_config(const char* config_file);
    
    void calculate_motor_commands(float roll, float pitch, float yaw, float throttle,
                                   float* motor_outputs);
    
    void set_motor_reversed(uint8_t motor_id, uint8_t reversed);
    uint8_t get_motor_reversed(uint8_t motor_id) const;
    
    const FrameConfig& get_current_frame() const { return current_frame; }
    const MotorMixing& get_mixing_matrix() const { return mixing_matrix; }
    
    static void build_vectored_frame(FrameConfig& frame);
    static void build_quadcopter_frame(FrameConfig& frame);
    static void build_hexacopter_frame(FrameConfig& frame);
    static void build_octocopter_frame(FrameConfig& frame);
    
private:
    FrameConfig current_frame;
    MotorMixing mixing_matrix;
    
    void recalculate_mixing_matrix();
};

