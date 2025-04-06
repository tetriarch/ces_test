#include "spell.hpp"
#include "components.hpp"

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData) {

    spellData_ = spellData;
}

void SpellComponent::attach() {

    currentDuration_ = 0.0f;
    traveledDistance_ = 0.0f;
    dead_ = false;
}

void SpellComponent::update(f32 dt) {

    auto oldPosition = entity()->transform().position;
    for(auto&& action : spellData_->actions) {
        action.motion->apply(entity(), dt);
    }
    auto newPosition = entity()->transform().position;

    currentDuration_ += dt;
    traveledDistance_ += Vec2(newPosition - oldPosition).length();
}

void SpellComponent::postUpdate(f32 dt) {

    if(dead_) {
        entity()->parent()->queueRemoveChild(entity());
        return;
    }

    if(spellData_->duration != 0) {
        if(currentDuration_ >= spellData_->duration) {
            dead_ = true;
        }
    }
    else if(traveledDistance_ >= spellData_->maxRange) {
        dead_ = true;
    }

    auto collisionComponent = entity()->component<CollisionComponent>();
    auto ownerComponent = entity()->component<OwnerComponent>();

    if(collisionComponent && ownerComponent) {
        if(collisionComponent->collided()) {
            auto collider = collisionComponent->collisionSource();
            if(!ownerComponent->isOwnedBy(collider)) {
                dead_ = true;
            }
        }
    }
}
