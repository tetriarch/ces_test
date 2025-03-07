#include "log.hpp"
#include "ui.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

UI::UI() : firstTime_(true) {}

UI::~UI() {

    if(imguiSDL3RendererInitResult_) ImGui_ImplSDLRenderer3_Shutdown();
    if(imguiSDL3InitResult_) ImGui_ImplSDL3_Shutdown();
    if(imguiContext_) ImGui::DestroyContext();
    imguiContext_ = nullptr;
}

bool UI::init(SDL_Window* window, SDL_Renderer* renderer) {

    // Setup Dear ImGui context
    imguiContext_ = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    imguiSDL3InitResult_ = ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    if(!imguiSDL3InitResult_) {
        ERROR("failed to init imgui for SDL renderer");
        return false;
    }
    imguiSDL3RendererInitResult_ = ImGui_ImplSDLRenderer3_Init(renderer);
    if(!imguiSDL3RendererInitResult_) {
        ERROR("failed to init imgui for SDL renderer");
        return false;
    }

    return true;
}

void UI::handleEvents(SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void UI::setupDockSpace() {

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags windowFlags;
    windowFlags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("dockspace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if(firstTime_) {

        firstTime_ = false;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID dockMain = dockspaceID;
        ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
        ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.15f, nullptr, &dockMain);

        ImGui::DockBuilderDockWindow("scene", dockLeft);
        ImGui::DockBuilderDockWindow("hud", dockBottom);

        ImGuiDockNode* nodeLeft = ImGui::DockBuilderGetNode(dockLeft);
        if(nodeLeft) {
            //NOTE: Ignore the warnings about combo of flag types
            nodeLeft->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }

        ImGuiDockNode* nodeBottom = ImGui::DockBuilderGetNode(dockBottom);
        if(nodeBottom) {
            nodeBottom->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::End();
}

void UI::render(SDL_Renderer* renderer) {

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    setupDockSpace();

    ImGuiWindowFlags windowFlags;
    windowFlags = ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoDecoration;


    ImGui::Begin("scene", nullptr, windowFlags);
    ImGui::Text("Scene hierarchy");
    ImGui::End();

    ImGui::Begin("hud", nullptr, windowFlags);
    ImGui::Text("This is where hud will be");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}


