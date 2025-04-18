#include "life.hpp"

#include "../entity.hpp"

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

void LifeComponent::regen(const f32 dt) {

    life_.current += life_.regen * dt;
}
