#include "asset_manager.hpp"
#include "core.hpp"
#include "components/components.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "log.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"
#include "spell_loader.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

constexpr std::string gameTitle = "CES_test";
constexpr s32 startWindowWidth = 1280;
constexpr s32 startWindowHeight = 720;

void indent(std::ostream& out, u32 depth) {

    for(u32 i = 0; i < depth * 1; i++) {
        out << "\t";
    }
    out << ">";
}

void print(std::ostream& out, const EntityPtr& e, u32 depth) {

    indent(out, depth);

    out << e->name() << " (" << e.get() << ")" << " [" << typeid(e).name() << "]" << std::endl;

    for(auto&& comp : e->components()) {
        indent(out, depth);
        out << comp->describe() << " " << comp.get() << std::endl;
    }

    for(auto&& child : e->children()) {
        print(out, child, depth + 1);
    }
}

Core::Core() : running_(true) {

}

Core::~Core() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Core::init() {

    if(!initSDL()) {
        ERROR("failed to init SDL");
        return false;
    }

    // game init
    AssetManager am("assets");
    am.registerLoader<SpellData>(std::make_shared<SpellLoader>());
    am.registerLoader<Scene>(std::make_shared<SceneLoader>());

    auto scene = am.load<Scene>("scenes/level_1.json");
    if(!scene) {
        return 1;
    }

    // output scene hierarchy
    print(std::cout, scene, 0);

    return true;
}

bool Core::initSDL() {

    if(!SDL_Init(SDL_INIT_VIDEO)) {
        ERROR(SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow(gameTitle.c_str(), startWindowWidth, startWindowHeight, 0);
    if(!window_) {
        ERROR(SDL_GetError());
        return false;
    }

    SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if(!renderer_) {
        ERROR(SDL_GetError());
        return false;
    }

    return true;
}

bool Core::initImGui() {

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2(startWindowWidth, startWindowHeight);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    if(!ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_)) {
        ERROR("failed to init imgui for SDL renderer");
        return false;
    }
    if(!ImGui_ImplSDLRenderer3_Init(renderer_)) {
        ERROR("failed to init imgui for SDL renderer");
        return false;
    }

    return true;
}

s32 Core::run() {

    if(!init()) {
        return 1;
    }

    if(!initImGui()) {
        return 1;
    }

    SDL_Event event;
    while(running_) {
        while(SDL_PollEvent(&event)) {
            handleEvents(event);
        }
        update();
        renderImGui();
        render();
    }
    return 0;
}

void Core::handleEvents(SDL_Event& event) {

    ImGui_ImplSDL3_ProcessEvent(&event);
    if(event.type == SDL_EVENT_QUIT) {
        running_ = false;
    }

}

void Core::update() {

}

void Core::renderImGui() {

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Dockspace", 0, ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(startWindowWidth, startWindowHeight));
    ImGuiID dockspaceID = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspaceID);
    ImGui::End();

    ImGui::Begin("Scene");
    ImGui::Text("Scene hierarchy");
    ImGui::End();

    ImGui::Render();
}

void Core::render() {

    SDL_RenderClear(renderer_);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
    SDL_RenderPresent(renderer_);

}

