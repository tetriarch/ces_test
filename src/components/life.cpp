#include "geometry.hpp"
#include "life.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"


const f32 LIFE_BAR_VERTICAL_OFFSET = 10;
const f32 LIFE_BAR_HEIGHT = 5;

void LifeComponent::attach() {
    dead = false;
}

const Life& LifeComponent::life() const {
    return life_;
}

void LifeComponent::setLife(const Life& life) {
    life_ = life;
}

void LifeComponent::reduceLife(f32 amount) {

    life_.current -= std::clamp(amount, 0.0f, life_.current);
}

void LifeComponent::increaseLife(f32 amount) {

    life_.current = std::min(amount + life_.current, life_.max);
}

bool LifeComponent::isAtFull() {

    return life_.current == life_.max;
}

void LifeComponent::update(const f32 dt) {

    if(life_.current < life_.max) {
        regen(dt);
    }
}

void LifeComponent::postUpdate(const f32 dt) {

    // clamp
    if(life_.current > life_.max) {
        life_.current = life_.max;
    }

    if(life_.current < 1.0f) {
        dead = true;
    }

    if(dead) {
        entity()->parent()->queueRemoveChild(entity());
    }
}

void LifeComponent::render(std::shared_ptr<Renderer> renderer) {

    renderer->queueRenderCall(Strata::UI, [&]() {
        auto geometryComponent = entity()->component<GeometryComponent>();
        if(geometryComponent) {
            Rect geometry = geometryComponent->rect();
            SDL_FRect rect = {
                geometry.x,
                geometry.y,
                geometry.w,
                geometry.h
            };

            auto t = life_.current / life_.max;

            SDL_FRect missingLifeBar = rect;
            missingLifeBar.y -= LIFE_BAR_VERTICAL_OFFSET;
            missingLifeBar.h = LIFE_BAR_HEIGHT;

            SDL_FRect currentLifeBar;
            currentLifeBar = missingLifeBar;
            currentLifeBar.w *= t;

            SDL_SetRenderDrawColor(renderer->handle(), 32, 0, 0, 255);
            SDL_RenderFillRect(renderer->handle(), &missingLifeBar);

            SDL_SetRenderDrawColor(renderer->handle(), 128, 0, 0, 255);
            SDL_RenderFillRect(renderer->handle(), &currentLifeBar);

            SDL_SetRenderDrawColor(renderer->handle(), 0, 0, 0, 255);
        }
    });
}


void LifeComponent::regen(const f32 dt) {

    life_.current += life_.regen * dt;
}
