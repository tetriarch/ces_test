#include "log.hpp"
#include "renderer.hpp"
#include "utils.hpp"

Renderer::Renderer(SDL_Renderer* renderer) {

    renderer_ = renderer;
}

void Renderer::destroy() {
    SDL_DestroyRenderer(renderer_);
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

SDL_Renderer* Renderer::handle() {
    return renderer_;
}

void Renderer::toogleVsync(bool toggle) {

    // vsync is disabled by default
    bool result = false;
    if(toggle) {
        result = SDL_SetRenderVSync(renderer_, 1);
    }
    else {
        SDL_SetRenderVSync(renderer_, 0);
    }

    if(!result) {
        INFO("failed to set v-sync");
    }
}

void Renderer::queueRenderCall(Strata strata, std::function<void()> renderCall) {

    switch(strata) {
        case Strata::TERRAIN:
            terrain_.push_back(renderCall);
            break;
        case Strata::ENTITY:
            entities_.push_back(renderCall);
            break;
        case Strata::EFFECT:
            effects_.push_back(renderCall);
            break;
        case Strata::UI:
            ui_.push_back(renderCall);
            break;
        default:
            break;
    }
}

void Renderer::executeRenderCalls() {

    renderGroup(terrain_);
    renderGroup(entities_);
    renderGroup(effects_);
    renderGroup(ui_);


}

void Renderer::present() {

    SDL_RenderPresent(renderer_);
}

void Renderer::renderGroup(const std::vector<std::function<void()>>& group) {

    for(auto& fn : group) {
        fn();
    }
}

void Renderer::clear() {

    terrain_.clear();
    entities_.clear();
    effects_.clear();
    ui_.clear();

    SDL_SetRenderDrawColor(renderer_, 0, 50, 0, 255);
    SDL_RenderClear(renderer_);
}
