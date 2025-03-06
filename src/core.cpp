#include "asset_manager.hpp"
#include "core.hpp"
#include "components/components.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "log.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"
#include "spell_loader.hpp"

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
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
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

    if(event.type == SDL_EVENT_KEY_DOWN) {
        if(event.key.key == SDLK_ESCAPE) {
            running_ = false;
        }
    }

}

void Core::update() {

}

void Core::render() {
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

}

