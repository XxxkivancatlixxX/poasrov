#pragma once
#include <SDL2/SDL.h>
#include <thread>
#include <atomic>

bool video_init(const char *rtsp_url, SDL_Renderer *renderer);
void video_init_async(const char *rtsp_url, SDL_Renderer *renderer);
void video_update();
SDL_Texture *video_get_texture();
void video_shutdown();
bool video_is_initialized();
