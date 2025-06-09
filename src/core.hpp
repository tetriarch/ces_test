#pragma once

#include <SDL3/SDL.h>

#include "renderer.hpp"
#include "utils.hpp"

class AssetManager;
class Scene;
class UI;
class Core {
public:
    Core();
    ~Core();

    s32 run();

private:
    bool init();
    bool initSDL();
    void handleEvents(const SDL_Event& event);
    void update(const f32 dt);
    void postUpdate(const f32 dt);
    void render();

private:
    SDL_Window* window_;
    SDL_Surface* icon_;
    std::shared_ptr<Renderer> renderer_;
    std::unique_ptr<UI> ui_;
    bool running_;
    std::shared_ptr<Scene> root_;
};
