#include "ui.hpp"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "components/life.hpp"
#include "components/mana.hpp"
#include "components/spell.hpp"
#include "components/spell_book.hpp"
#include "components/tag.hpp"
#include "components/xp.hpp"
#include "log.hpp"
#include "renderer.hpp"
#include "scene.hpp"

const std::string SPELL_SLOT_DEFAULT = "Select Spell";

UI::UI() : firstTime_(true) {
#ifdef DEBUG
    showScene_ = true;
    showDemoWindow_ = false;
#endif
}

UI::~UI() {
    if(imguiSDL3RendererInitResult_) ImGui_ImplSDLRenderer3_Shutdown();
    if(imguiSDL3InitResult_) ImGui_ImplSDL3_Shutdown();
    if(imguiContext_) ImGui::DestroyContext();
    imguiContext_ = nullptr;
}

bool UI::init(SDL_Window* window, std::shared_ptr<Renderer> renderer) {
    renderer_ = renderer;

    // Setup Dear ImGui context
    imguiContext_ = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = {0.0f, 0.0f, 0.0f, 0.0f};

    imguiSDL3InitResult_ = ImGui_ImplSDL3_InitForSDLRenderer(window, renderer->handle());
    if(!imguiSDL3InitResult_) {
        ERROR("[UI]: failed to init imgui for SDL renderer");
        return false;
    }
    imguiSDL3RendererInitResult_ = ImGui_ImplSDLRenderer3_Init(renderer->handle());
    if(!imguiSDL3RendererInitResult_) {
        ERROR("[UI]: failed to init imgui for SDL renderer");
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
    windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                  ImGuiWindowFlags_NoInputs;

    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::GetStyle().FrameBorderSize = 0.0f;
    ImGui::GetStyle().DockingSeparatorSize = 0.0f;

    ImGuiID dockspaceID = ImGui::GetID("dockspace");
    ImGui::DockSpaceOverViewport(dockspaceID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if(firstTime_) {
        firstTime_ = false;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID dockMain = dockspaceID;
        ImGuiID dockBottom =
            ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.15f, nullptr, &dockMain);

#ifdef DEBUG
        ImGuiID dockLeft =
            ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.2f, nullptr, &dockMain);
        ImGui::DockBuilderDockWindow("scene", dockLeft);
        ImGuiDockNode* nodeLeft = ImGui::DockBuilderGetNode(dockLeft);
        if(nodeLeft) {
            // NOTE: Ignore the warnings about combo of flag types
            nodeLeft->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }
#endif

        ImGui::DockBuilderDockWindow("hud", dockBottom);
        ImGuiDockNode* nodeBottom = ImGui::DockBuilderGetNode(dockBottom);
        if(nodeBottom) {
            // NOTE: Ignore the warnings about combo of flag types
            nodeBottom->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize;
        }

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::End();
}

void UI::render(std::shared_ptr<Scene> scene) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    setupDockSpace();

#ifdef DEBUG
    if(showScene_) renderSceneHierarchy(scene);
    if(showDemoWindow_) ImGui::ShowDemoWindow();
#endif

    // go through children and find player's spellbook
    EntityPtr player = nullptr;
    for(auto& c : scene->children()) {
        auto tag = c->component<TagComponent>();
        if(tag && tag->tag() == TagType::PLAYER) {
            player = c;
        }
    }

    renderHUD(player);

    ImGui::Render();

    auto renderer = renderer_.lock();

    if(!renderer) {
        ERROR_ONCE("[UI]: unable to access renderer");
        return;
    }

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer->handle());
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

void UI::renderHUD(EntityPtr player) {
    if(!player) {
        ERROR_ONCE("[UI]: no player to render hud");
        return;
    }
    ImGui::Begin("hud", nullptr, 0);

    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing = ImVec2(0, 0);
    style.WindowPadding = ImVec2(0, 0);

    auto spellBook = player ? player->component<SpellBookComponent>() : nullptr;
    auto life = player ? player->component<LifeComponent>() : nullptr;
    auto mana = player ? player->component<ManaComponent>() : nullptr;
    auto xp = player ? player->component<XPComponent>() : nullptr;

    // we need xp to get level value
    if(!xp) {
        ERROR_ONCE("[UI]: no xp component to render hud");
        return;
    }
    u32 level = xp->level();
    std::string playerText = player->name() + " - level " + std::to_string(level);
    ImVec2 playerTextSize = ImGui::CalcTextSize(playerText.c_str());

    f32 windowWidth = ImGui::GetWindowWidth();
    f32 uiWidth = 750;
    f32 horizontalCenter = (windowWidth - uiWidth) * 0.5f;
    f32 playerNameHorizontalCenter = (windowWidth - playerTextSize.x) * 0.5f;

    ImVec2 resourceBarSize = {uiWidth / 2.0f, 20};
    ImVec2 xpBarSize = {uiWidth, 15};
    ImVec2 castBarSize = {uiWidth, 15};

    ImGui::SetCursorPosX(playerNameHorizontalCenter);
    ImGui::Text("%s", playerText.c_str());

    // render cast bar
    if(!spellBook) {
        ERROR_ONCE("[UI]: no spellbook component to render hud");
        return;
    }
    if(spellBook->castedSpell()) {
        ImGui::SetCursorPosX(horizontalCenter);
        if(spellBook->interruptible()) {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.4f, 0.00f, 0.4f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        auto spellName = spellBook->castedSpell() ? spellBook->castedSpell()->name.c_str() : "";
        ImVec2 spellTextSize = ImGui::CalcTextSize(spellName);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 spellTextPosition = ImVec2(cursorPos.x + (castBarSize.x - spellTextSize.x) * 0.5f,
            cursorPos.y + (castBarSize.y - spellTextSize.y) * 0.5f);
        ImGui::ProgressBar(spellBook->castProgress(), castBarSize, "");
        ImGui::GetWindowDrawList()->AddText(
            spellTextPosition, IM_COL32(255, 255, 255, 255), spellName);
        ImGui::PopStyleColor();
    } else {
        ImGui::Dummy(castBarSize);
    }

    // render xp bar
    ImGui::SetCursorPosX(horizontalCenter);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    bool maxLevel = xp->isMaxLevel();
    u32 currentXP = xp->currentXP();
    u32 nextLevelXP = xp->nextLevelXP();
    f32 progress = maxLevel ? 1.0f : static_cast<f32>(currentXP) / static_cast<f32>(nextLevelXP);

    std::string xpText;
    if(maxLevel) {
        xpText = "MAXIMUM LEVEL";
    } else {
        xpText = std::to_string(currentXP) + " / " + std::to_string(nextLevelXP);
    }
    ImVec2 xpTextSize = ImGui::CalcTextSize(xpText.c_str());
    ImVec2 xpTextPosition = ImVec2(cursorPos.x + (xpBarSize.x - xpTextSize.x) * 0.5f,
        cursorPos.y + (xpBarSize.y - xpTextSize.y) * 0.5f);

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.73f, 0.43f, 0.0f, 1.0f));
    ImGui::ProgressBar(progress, xpBarSize, "");
    ImGui::GetWindowDrawList()->AddText(
        xpTextPosition, IM_COL32(255, 255, 255, 255), xpText.c_str());
    ImGui::PopStyleColor();

    Life lifeValue;
    Mana manaValue;
    if(!life) {
        ERROR_ONCE("[UI]: no life component to render hud");
        return;
    }
    lifeValue = life->life();
    auto lifeCurrent = static_cast<u32>(std::floor(lifeValue.current));
    auto lifeMax = static_cast<u32>(std::floor(lifeValue.max));
    std::string lifeText;
    bool dead = life->isDead();
    if(dead) {
        lifeText = "DEAD";
    } else {
        lifeText = std::to_string(lifeCurrent) + "/" + std::to_string(lifeMax) + " ";
    }
    ImVec2 lifeTextSize = ImGui::CalcTextSize(lifeText.c_str());

    if(!mana) {
        ERROR_ONCE("[UI]: no mana component to render hud");
        return;
    }
    manaValue = mana->mana();
    auto manaCurrent = static_cast<u32>(std::floor(manaValue.current));
    auto manaMax = static_cast<u32>(std::floor(manaValue.max));
    std::string manaText = " " + std::to_string(manaCurrent) + "/" + std::to_string(manaMax);
    ImVec2 manaTextSize = ImGui::CalcTextSize(manaText.c_str());

    // render resources
    ImGui::SetCursorPosX(horizontalCenter);
    if(life && mana) {
        {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
            cursorPos = ImGui::GetCursorScreenPos();
            ImVec2 lifeTextPosition =
                ImVec2(cursorPos.x + (resourceBarSize.x - lifeTextSize.x) * 0.5f,
                    cursorPos.y + (resourceBarSize.y - lifeTextSize.y) * 0.5f);
            ImGui::ProgressBar(lifeValue.current / lifeValue.max, resourceBarSize, "");
            if(dead) {
                ImGui::GetWindowDrawList()->AddText(
                    lifeTextPosition, IM_COL32(255, 0, 0, 255), lifeText.c_str());
            } else {
                ImGui::GetWindowDrawList()->AddText(
                    lifeTextPosition, IM_COL32(255, 255, 255, 255), lifeText.c_str());
            }
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImVec2 manaTextPosition =
                ImVec2(cursorPos.x + (resourceBarSize.x - manaTextSize.x) * 0.5f,
                    cursorPos.y + (resourceBarSize.y - manaTextSize.y) * 0.5f);
            ImGui::ProgressBar(manaValue.current / manaValue.max, resourceBarSize, "");
            ImGui::GetWindowDrawList()->AddText(
                manaTextPosition, IM_COL32(255, 255, 255, 255), manaText.c_str());
            ImGui::PopStyleColor();
        }
    }

    // render spell slots
    ImVec2 spellSlotSize = {uiWidth / 4.0f, 20.0f};
    ImGui::SetCursorPosX(horizontalCenter);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    if(ImGui::BeginTable("##SpellSlots", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("##Slot 1", ImGuiTableColumnFlags_WidthFixed, spellSlotSize.x);
        ImGui::TableSetupColumn("##Slot 2", ImGuiTableColumnFlags_WidthFixed, spellSlotSize.x);
        ImGui::TableSetupColumn("##Slot 3", ImGuiTableColumnFlags_WidthFixed, spellSlotSize.x);
        ImGui::TableSetupColumn("##Slot 4", ImGuiTableColumnFlags_WidthFixed, spellSlotSize.x);
        ImGui::TableNextRow();

        for(u32 i = 0; i < 4; i++) {
            ImGui::TableSetColumnIndex(i);
            auto spellSlots = spellBook->slots();
            if(!spellSlots[i]) {
                selectedSpells_[i] = SPELL_SLOT_DEFAULT;
            } else {
                selectedSpells_[i] = spellSlots[i]->name;
            }

            // render spell cooldown
            f32 cooldown;
            f32 cooldownProgress;
            ImGui::SetNextItemWidth(spellSlotSize.x);
            if(spellBook->isSpellInSlotOnCooldown(i, &cooldown, &cooldownProgress)) {
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
                std::string cooldownText = std::format("{:.2f}s", cooldown);
                ImVec2 cooldownTextSize = ImGui::CalcTextSize(cooldownText.c_str());
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                ImVec2 spellTextPosition =
                    ImVec2(cursorPos.x + (spellSlotSize.x - cooldownTextSize.x) * 0.5f,
                        cursorPos.y + (spellSlotSize.y - cooldownTextSize.y) * 0.5f);
                ImGui::ProgressBar(cooldownProgress, spellSlotSize, "");
                ImGui::GetWindowDrawList()->AddText(
                    spellTextPosition, IM_COL32(255, 255, 255, 255), cooldownText.c_str());
                ImGui::PopStyleColor();
            } else {
                // render spell selection
                std::string slotLabel = "##slot " + std::to_string(i + 1);
                auto& selectedSpell = selectedSpells_[i];
                bool clearSelected = false;
                if(ImGui::BeginCombo(slotLabel.c_str(), selectedSpell.c_str())) {
                    if(spellBook) {
                        for(auto& spell : spellBook->spells()) {
                            bool onCooldown = spellBook->isSpellOnCooldown(spell);
                            bool selected = spell->name == selectedSpell;
                            bool canSelect = true;
                            // grey out unavailable spells
                            for(u32 j = 0; j < selectedSpells_.size(); j++) {
                                if(i != j && selectedSpells_[j] == spell->name) {
                                    canSelect = false;
                                    break;
                                }
                            }
                            if(onCooldown || !canSelect) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0f));
                                ImGui::Selectable(
                                    spell->name.c_str(), &selected, ImGuiSelectableFlags_Disabled);
                                ImGui::PopStyleColor();
                            }
                            // normal selection
                            else {
                                if(ImGui::Selectable(spell->name.c_str(), &selected)) {
                                    selectedSpell = spell->name;
                                }

                                if(selected) {
                                    ImGui::SetItemDefaultFocus();
                                    spellBook->setSlot(i, spell);
                                }
                            }
                        }
                        // add clear slot option
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0f));
                        if(ImGui::Selectable("X Clear Slot X", &clearSelected)) {
                            selectedSpell = SPELL_SLOT_DEFAULT;
                            spellBook->setSlot(i, nullptr);
                        }
                        ImGui::PopStyleColor();
                    }
                    ImGui::EndCombo();
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}
