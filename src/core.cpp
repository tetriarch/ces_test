#include "core.hpp"

#include "asset_manager.hpp"
#include "entity_structure_modifier.hpp"
#include "loaders/animation_loader.hpp"
#include "loaders/emitter_loader.hpp"
#include "loaders/entity_loader.hpp"
#include "loaders/particle_loader.hpp"
#include "loaders/scene_loader.hpp"
#include "loaders/spell_loader.hpp"
#include "loaders/status_effect_loader.hpp"
#include "loaders/texture_loader.hpp"
#include "log.hpp"
#include "time.hpp"
#include "ui.hpp"

constexpr std::string gameTitle = "CES_test";
constexpr s32 startWindowWidth = 1280;
constexpr s32 startWindowHeight = 720;

Core::Core() : running_(true) {
    ui_ = std::make_unique<UI>();
}

Core::~Core() {
    renderer_->destroy();
    SDL_DestroySurface(icon_);
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
    auto am = AssetManager::get();
    am->setAssetRoot("assets");
    am->registerLoader<SpellData>(std::make_shared<SpellLoader>());
    am->registerLoader<Scene>(std::make_shared<SceneLoader>());
    am->registerLoader<EntityData>(std::make_shared<EntityLoader>());
    am->registerLoader<Texture>(std::make_shared<TextureLoader>(renderer_));
    am->registerLoader<AnimationData>(std::make_shared<AnimationLoader>());
    am->registerLoader<StatusEffectData>(std::make_shared<StatusEffectLoader>());
    am->registerLoader<EmitterData>(std::make_shared<EmitterLoader>());
    am->registerLoader<ParticleData>(std::make_shared<ParticleLoader>());

    root_ = am->load<Scene>("scenes/level_1.json");

    // Add the collision system to the root.
    root_->addComponent(std::make_shared<CollisionSystem>());

    if(!root_) {
        return false;
    }

    root_->executeAttached();

    return true;
}

bool Core::initSDL() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        ERROR(SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow(
        gameTitle.c_str(), startWindowWidth, startWindowHeight, SDL_WINDOW_RESIZABLE);
    if(!window_) {
        ERROR(SDL_GetError());
        return false;
    }

    SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    icon_ = SDL_LoadBMP("assets/app_icon.bmp");
    if(!icon_) {
        ERROR(SDL_GetError());
        return false;
    }
    SDL_SetWindowIcon(window_, icon_);

    renderer_ = std::make_shared<Renderer>(Renderer::init(window_));

    renderer_->toogleVsync(true);

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
        Time::get().update();

        while(Time::get().isTimeToUpdate()) {
            // Apply all the modifications queued from the previous frame
            EntityStructureModifier::applyStructureModifications();
            {
                // Begin our update cycle
                EntityStructureModifier::beginUpdate();
                update(Time::get().DELTA_TIME);
                postUpdate(Time::get().DELTA_TIME);
                EntityStructureModifier::endUpdate();
            }
        }
        render();
    }
    return 0;
}

void Core::handleEvents(const SDL_Event& event) {
    root_->handleEvents(event);
    ui_->handleEvents(event);

    if(event.type == SDL_EVENT_QUIT) {
        running_ = false;
    }
}

void Core::update(const f32 dt) {
    root_->update(dt);
}

void Core::postUpdate(const f32 dt) {
    root_->postUpdate(dt);
}

void Core::render() {
    renderer_->clear();
    root_->render(renderer_);
    renderer_->executeRenderCalls();
    ui_->render(root_);
    renderer_->present();
}
