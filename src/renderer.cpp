#include "log.hpp"
#include "renderer.hpp"
#include "utils.hpp"

Renderer::Renderer(SDL_Renderer* renderer) {

    renderer_ = renderer;
}

Renderer Renderer::init(SDL_Window* window) {

    SDL_Renderer* r = SDL_CreateRenderer(window, nullptr);
    if(!r) {
        std::string error = SDL_GetError();
        FATAL_ERROR("[RENDERER]: failed to init - " + error);
    }

    Renderer renderer(r);
    return renderer;
}
