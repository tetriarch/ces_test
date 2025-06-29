#include "life.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "geometry.hpp"
#include "reward.hpp"
#include "tag.hpp"
#include "xp.hpp"

const f32 LIFE_PER_LEVEL_INCREMENT = 10.0f;
const f32 LIFE_BAR_VERTICAL_OFFSET = 10;
const f32 LIFE_BAR_HEIGHT = 5;

LifeComponent::LifeComponent() : life_{0.0f, 0.0f}, dead_(false) {
}

void LifeComponent::attach() {
    reset();
}

const Life& LifeComponent::life() const {
    return life_;
}

void LifeComponent::setLife(const Life& life) {
    life_ = life;
}

void LifeComponent::reset() {
    auto xpComponent = entity()->component<XPComponent>();
    if(xpComponent) {
        u32 level = xpComponent->level();
        life_.max += LIFE_PER_LEVEL_INCREMENT * (level - 1);
        life_.current = life_.max;
    }
}

void LifeComponent::reduceLife(f32 amount, EntityPtr applier) {
    life_.current -= std::clamp(amount, 0.0f, life_.current);
    lastAttacker_ = applier;
}

void LifeComponent::increaseLife(f32 amount) {
    assert(amount > 0);
    life_.current = std::min(amount + life_.current, life_.max);
}

bool LifeComponent::isAtFull() {
    return life_.current == life_.max;
}

bool LifeComponent::isDead() const {
    return dead_;
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

    auto tagComponent = entity()->component<TagComponent>();
    if(life_.current < 0.1f) {
        auto lastAttacker = lastAttacker_.lock();
        if(lastAttacker && tagComponent && tagComponent->tag() == TagType::PLAYER) {
            INFO("[LIFE]: " + entity()->name() + " was killed by " + lastAttacker_.lock()->name());
        } else {
            INFO("[LIFE]: " + entity()->name() + " died!");
        }
        dead_ = true;
    }

    if(dead_) {
        auto lastAttacker = lastAttacker_.lock();
        if(lastAttacker) {
            auto xpComponent = entity()->component<XPComponent>();
            auto rewardComponent = entity()->component<RewardComponent>();
            auto attackerXPComponent = lastAttacker->component<XPComponent>();
            if(xpComponent && rewardComponent && attackerXPComponent) {
                attackerXPComponent->gainXP(xpComponent->level(), rewardComponent->xp());
            }
        }
        if(tagComponent->tag() == TagType::PLAYER) {
            entity()->setActive(false);
        } else {
            entity()->parent()->queueRemoveChild(entity());
        }
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
