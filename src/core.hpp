#pragma once

#include "utils.hpp"

#include <SDL3/SDL.h>

class Core {

public:
    Core();
    ~Core();

    s32 run();

private:
    bool init();
    bool initSDL();
    void handleEvents(SDL_Event& event);
    void update();
    void render();

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
};