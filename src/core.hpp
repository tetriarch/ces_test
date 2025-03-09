#pragma once

#include "utils.hpp"

#include <SDL3/SDL.h>

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
    void update();
    void render();

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    std::unique_ptr<UI> ui_;
    bool running_;
    std::weak_ptr<Scene> root_;
};