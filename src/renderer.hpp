#pragma once

#include <SDL3/SDL.h>

class Renderer {

public:
    Renderer(SDL_Renderer* renderer);
    static Renderer init(SDL_Window* window);

private:
    SDL_Renderer* renderer_;
};