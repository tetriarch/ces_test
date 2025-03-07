#pragma once

#include "utils.hpp"

#include <SDL3/SDL.h>

class ImGuiContext;
class UI {

public:
    UI();
    ~UI();

    bool init(SDL_Window* window, SDL_Renderer* renderer);
    void handleEvents(SDL_Event& event);
    void render(SDL_Renderer* renderer);

private:
    void setupDockSpace();

private:
    ImGuiContext* imguiContext_;
    bool imguiSDL3InitResult_;
    bool imguiSDL3RendererInitResult_;
    bool firstTime_;
};