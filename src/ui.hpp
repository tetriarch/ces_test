#pragma once

#include "utils.hpp"

#include <SDL3/SDL.h>

struct ImGuiContext;
class Scene;
class Renderer;

class UI {

public:
    UI();
    ~UI();

    bool init(SDL_Window* window, std::shared_ptr<Renderer> renderer);
    void handleEvents(const SDL_Event& event);
    void render(std::shared_ptr<Scene> scene);
    void renderSceneHierarchy(std::shared_ptr<Scene> scene);
    void renderHUD(EntityPtr player);



private:
    void setupDockSpace();

private:
    std::weak_ptr<Renderer> renderer_;
    ImGuiContext* imguiContext_;
    bool imguiSDL3InitResult_;
    bool imguiSDL3RendererInitResult_;
    bool firstTime_;
    std::array<std::string, 4> selectedSpells_;

#ifdef DEBUG
    bool showScene_;
    bool showDemoWindow_;
#endif 
};