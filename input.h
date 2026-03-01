#pragma once
#include <SDL2/SDL.h>

struct ControllerState {
    bool  connected = false;
    float axis_left_x = 0.0f;
    float axis_left_y = 0.0f;
    float axis_right_x = 0.0f;
    float axis_right_y = 0.0f;
    float trigger_left = 0.0f;
    float trigger_right = 0.0f;
    bool  button_a = false;
    bool  button_b = false;
    bool  button_x = false;
    bool  button_y = false;
    bool  button_start = false;
    bool  button_back = false;
};

void input_init();
void input_handle_event(const SDL_Event &e);
void input_update();
const ControllerState &input_get_state();
