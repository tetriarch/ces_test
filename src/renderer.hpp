#pragma once

#include <SDL3/SDL.h>
#include <functional>

enum class Strata {
    TERRAIN = 1,
    ENTITY = 2,
    EFFECT = 3,
    UI = 4
};

class Renderer {

public:
    Renderer(SDL_Renderer* renderer);
    void destroy();
    static Renderer init(SDL_Window* window);

public:
    SDL_Renderer* handle();;
    void toogleVsync(bool toggle);
    void queueRenderCall(Strata strata, std::function<void()> renderCall);
    void executeRenderCalls();
    void present();
    void clear();

private:
    void renderGroup(const std::vector<std::function<void()>>& group);

private:
    SDL_Renderer* renderer_;

private:
    std::vector<std::function<void()>> terrain_;
    std::vector<std::function<void()>> entities_;
    std::vector<std::function<void()>> effects_;
    std::vector<std::function<void()>> ui_;
};