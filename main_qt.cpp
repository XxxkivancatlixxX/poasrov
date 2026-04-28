// çat çat kameralarımızdasınız
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <SDL2/SDL.h>
#include <cstdio>

#include "Backend.h"
#include "camera_image_provider.h"

int main(int argc, char *argv[])
{
    // Initialize SDL for joystick/gamepad support
    if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        std::fprintf(stderr, "Warning: Controller support will not be available\n");
    } else {
        std::fprintf(stderr, "SDL initialized successfully\n");
        std::fprintf(stderr, "Number of joysticks detected: %d\n", SDL_NumJoysticks());
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            if (SDL_IsGameController(i)) {
                std::fprintf(stderr, "  Controller %d: %s\n", i, SDL_GameControllerNameForIndex(i));
            } else {
                std::fprintf(stderr, "  Joystick %d: %s (not a game controller)\n", i, SDL_JoystickNameForIndex(i));
            }
        }
    }

    QGuiApplication app(argc, argv);

    Backend backend;

    QQmlApplicationEngine engine;
    
    // kamerasal
    engine.addImageProvider("camera", new CameraImageProvider(&backend));
    
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.load(QUrl::fromLocalFile("qml/Main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    int result = app.exec();
    
    SDL_Quit();
    return result;
}

