#pragma once

#include "utils.hpp"

#include <SDL3/SDL.h>

class ImGuiContext;
class Scene;
class UI {

public:
    UI();
    ~UI();

    bool init(SDL_Window* window, SDL_Renderer* renderer);
    void handleEvents(const SDL_Event& event);
    void render(SDL_Renderer* renderer, std::shared_ptr<Scene> scene);
    void renderSceneHierarchy(std::shared_ptr<Scene> scene);



private:
    void setupDockSpace();

private:
    ImGuiContext* imguiContext_;
    bool imguiSDL3InitResult_;
    bool imguiSDL3RendererInitResult_;
    bool firstTime_;

#ifdef DEBUG
    bool showScene_;
    bool showDemoWindow_;
#endif 
};