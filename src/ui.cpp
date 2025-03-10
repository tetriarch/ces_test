#include "log.hpp"
#include "scene.hpp"
#include "ui.hpp"
#include "components/spell.hpp"
#include "components/spell_book.hpp"
#include "components/tag.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

UI::UI() :
    firstTime_(true) {

#ifdef DEBUG
    showScene_ = true;
    showDemoWindow_ = false;
#endif
    selectedSpell0_ = "";
    selectedSpell1_ = "";
    selectedSpell2_ = "";
    selectedSpell3_ = "";

}

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
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = {0.0f, 0.0f, 0.0f, 0.0f};

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

void UI::handleEvents(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
#ifdef DEBUG
    if(event.type == SDL_EVENT_KEY_DOWN) {
        if(event.key.key == SDLK_F12 && event.key.mod & SDL_KMOD_LCTRL) {
            showScene_ = !showScene_;
        }
        if(event.key.key == SDLK_F11 && event.key.mod & SDL_KMOD_LCTRL) {
            showDemoWindow_ = !showDemoWindow_;
        }
    }
#endif
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
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("dockspace");
    ImGui::DockSpaceOverViewport(dockspaceID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if(firstTime_) {

        firstTime_ = false;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID dockMain = dockspaceID;
        ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.15f, nullptr, &dockMain);

    #ifdef DEBUG
        ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
        ImGui::DockBuilderDockWindow("scene", dockLeft);
        ImGuiDockNode* nodeLeft = ImGui::DockBuilderGetNode(dockLeft);
        if(nodeLeft) {
            //NOTE: Ignore the warnings about combo of flag types
            nodeLeft->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }
    #endif

        ImGui::DockBuilderDockWindow("hud", dockBottom);
        ImGuiDockNode* nodeBottom = ImGui::DockBuilderGetNode(dockBottom);
        if(nodeBottom) {
            nodeBottom->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::End();
}

void UI::render(SDL_Renderer* renderer, std::shared_ptr<Scene> scene) {

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    setupDockSpace();

#ifdef DEBUG
    if(showScene_) renderSceneHierarchy(scene);
    if(showDemoWindow_) ImGui::ShowDemoWindow();
#endif

    // go through children and find player's spellbook
    std::shared_ptr<SpellBookComponent> spellBook = nullptr;
    for(auto& c : scene->children()) {
        auto tag = c->component<TagComponent>();
        if(tag && tag->tag() == TagType::PLAYER) {
            spellBook = c->component<SpellBookComponent>();
        }
    }

    ImGui::Begin("hud", nullptr, 0);

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 250);
    if(ImGui::BeginCombo("slot 1", selectedSpell0_.c_str())) {
        if(spellBook) {
            for(auto& spell : spellBook->spells()) {
                bool selected = spell->name == selectedSpell0_;
                if(ImGui::Selectable(spell->name.c_str(), &selected)) {
                    selectedSpell0_ = spell->name;
                }
                if(selected) {
                    ImGui::SetItemDefaultFocus();
                    spellBook->setSlot(0, spell);
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, 250);

    if(ImGui::BeginCombo("slot 2", selectedSpell1_.c_str())) {
        if(spellBook) {
            for(auto& spell : spellBook->spells()) {
                bool selected = spell->name == selectedSpell1_;
                if(ImGui::Selectable(spell->name.c_str(), &selected)) {
                    selectedSpell1_ = spell->name;
                }
                if(selected) {
                    ImGui::SetItemDefaultFocus();
                    spellBook->setSlot(1, spell);
                }
            }
        }
        ImGui::EndCombo();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void UI::renderSceneHierarchy(std::shared_ptr<Scene> scene) {

    ImGui::Begin("scene", nullptr, 0);

    ImGui::SeparatorText("Scene Hierarchy");
    if(ImGui::TreeNode(scene->name().c_str())) {

        for(auto& c : scene->children()) {
            if(ImGui::TreeNode(c->name().c_str())) {
                for(auto& comp : c->components()) {
                    ImGui::Text("%s", comp->componentType().name());
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}


