#include "asset_manager.hpp"
#include "core.hpp"
#include "components/components.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "log.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"
#include "spell_loader.hpp"
#include "ui.hpp"

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

    ui_ = std::make_unique<UI>();

}

Core::~Core() {

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Core::init() {

    if(!initSDL()) {
        ERROR("failed to init SDL");
        return false;
    }

    if(!ui_->init(window_, renderer_)) {
        ERROR("failed to init UI");
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

s32 Core::run() {

    if(!init()) {
        return 1;
    }

    SDL_Event event;
    while(running_) {
        while(SDL_PollEvent(&event)) {
            handleEvents(event);
        }
        update();
        render();
    }
    return 0;
}

void Core::handleEvents(SDL_Event& event) {

    ui_->handleEvents(event);
    if(event.type == SDL_EVENT_QUIT) {
        running_ = false;
    }

}

void Core::update() {

}

void Core::render() {

    SDL_RenderClear(renderer_);
    ui_->render(renderer_);
    SDL_RenderPresent(renderer_);
}

