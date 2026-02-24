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
