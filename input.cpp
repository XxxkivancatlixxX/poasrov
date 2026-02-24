#include "input.h"
#include <SDL2/SDL.h>
#include <math.h>

static SDL_GameController *g_pad = NULL;
static ControllerState g_state = {0};

static float axis_to_float(Sint16 v)
{
    float f = v / 32767.0f;
    if (fabsf(f) < 0.05f) f = 0.0f;
    return f;
}

void input_init(void)
{
    int num = SDL_NumJoysticks();
    for (int i = 0; i < num; ++i) {
        if (SDL_IsGameController(i)) {
            g_pad = SDL_GameControllerOpen(i);
            if (g_pad) {
                g_state.connected = true;
                break;
            }
        }
    }
}

void input_handle_event(const SDL_Event &e)
{
    if (e.type == SDL_CONTROLLERDEVICEADDED) {
        if (!g_pad && SDL_IsGameController(e.cdevice.which)) {
            g_pad = SDL_GameControllerOpen(e.cdevice.which);
            if (g_pad) g_state.connected = true;
        }
    } else if (e.type == SDL_CONTROLLERDEVICEREMOVED) {
        if (g_pad && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(g_pad)) == e.cdevice.which) {
            SDL_GameControllerClose(g_pad);
            g_pad = NULL;
            g_state.connected = false;
        }
    }
}

void input_update()
{
    if (!g_pad) return;

    g_state.axis_left_x  = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTX));
    g_state.axis_left_y  = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTY));
    g_state.axis_right_x = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_RIGHTX));
    g_state.axis_right_y = axis_to_float(SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_RIGHTY));
    g_state.trigger_left  = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT)  / 32767.0f;
    g_state.trigger_right = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;

    g_state.button_a     = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_A);
    g_state.button_b     = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_B);
    g_state.button_x     = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_X);
    g_state.button_y     = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_Y);
    g_state.button_start = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_START);
    g_state.button_back  = SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_BACK);
}

const ControllerState &input_get_state()
{
    return g_state;
}
