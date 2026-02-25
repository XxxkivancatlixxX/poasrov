#pragma once
#include <SDL2/SDL.h>
#include "input.h"

void ui_init(SDL_Window *window, SDL_Renderer *renderer);
void ui_process_event(const SDL_Event &e);
void ui_new_frame();
void ui_draw(const ControllerState &ctrl, SDL_Texture *video_tex);
void ui_render();
void ui_shutdown();
void ui_log(const char *message);
void ui_send_control_packet(const ControllerState &ctrl);
void ui_receive_telemetry();  // Send control data to firmware
bool ui_connect_to_pixhawk(const char* host, uint16_t port);
bool ui_is_connected_to_pixhawk();
