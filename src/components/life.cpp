#include "life.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "geometry.hpp"

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
        INFO("[LIFE]: " + entity()->name() + " died!");
        dead = true;
    }

    if(dead) {
        entity()->parent()->queueRemoveChild(entity());
    }
}

void LifeComponent::render(std::shared_ptr<Renderer> renderer) {
    auto geometryComponent = entity()->component<GeometryComponent>();
    if(geometryComponent) {
        auto t = life_.current / life_.max;
        Rect rect = geometryComponent->rect();

        Rect missingLifeBar = rect;
        missingLifeBar.y -= LIFE_BAR_VERTICAL_OFFSET;
        missingLifeBar.h = LIFE_BAR_HEIGHT;

        Rect currentLifeBar;
        currentLifeBar = missingLifeBar;
        currentLifeBar.w *= t;

        renderer->queueRenderFilledRect(Strata::UI, missingLifeBar, 32, 0, 0, 255);
        renderer->queueRenderFilledRect(Strata::UI, currentLifeBar, 128, 0, 0, 255);
    }
}

void LifeComponent::regen(const f32 dt) {
    life_.current += life_.regen * dt;
}
