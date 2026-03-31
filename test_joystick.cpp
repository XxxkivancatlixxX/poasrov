// test of za controller 
#include "input.h"
#include <SDL2/SDL.h>
#include <cstdio>
#include <unistd.h>

int main() {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    input_init();
    
    printf("Joystick test - press Ctrl+C to exit\n");
    printf("Plug in a controller if not already connected\n\n");
    
    SDL_Event e;
    for (int i = 0; i < 300; i++) {  // Run for ~30 seconds
        while (SDL_PollEvent(&e)) {
            input_handle_event(e);
        }
        
        input_update();
        const ControllerState& state = input_get_state();
        
        printf("\r[%s] LX:%.2f LY:%.2f RX:%.2f RY:%.2f LT:%.2f RT:%.2f A:%d B:%d X:%d Y:%d   ",
               state.connected ? "CONNECTED" : "NO DEVICE",
               state.axis_left_x, state.axis_left_y,
               state.axis_right_x, state.axis_right_y,
               state.trigger_left, state.trigger_right,
               state.button_a, state.button_b, state.button_x, state.button_y);
        fflush(stdout);
        
        usleep(100000);  // 100ms = 10Hz
    }
    
    printf("\n\nTest complete\n");
    SDL_Quit();
    return 0;
}
