#include "../entity.hpp"

#include "spell_book.hpp"
#include "velocity.hpp"

Vec2 VelocityComponent::velocity() const {

    Vec2 velocityVector = {0,0};
    if((movementDirection_ & static_cast<u8>(MovementDirection::NORTH)) != 0) {
        velocityVector += {0, -1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::EAST)) != 0) {
        velocityVector += {1, 0};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::SOUTH)) != 0) {
        velocityVector += {0, 1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::WEST)) != 0) {
        velocityVector += {-1, 0};
    }

    f32 speed = speed_;
    // slow down on casting
    //NOTE: hardcoded multiplier
    auto spellBook = entity()->component<SpellBookComponent>();
    if(spellBook && spellBook->isCasting()) {
        speed *= 0.75f;
    }

    return velocityVector.normalized() * speed;
}

void VelocityComponent::update(const f32 dt) {

    auto transform = entity()->transform();
    transform.position += velocity() * dt;
    entity()->setTransform(transform);
}

f32 VelocityComponent::speed() const {

    return speed_;
}

void VelocityComponent::setMotion(MovementDirection direction) {

    movementDirection_ |= static_cast<u8>(direction);
}

void VelocityComponent::stopMotion(MovementDirection direction) {

    movementDirection_ &= ~static_cast<u8>(direction);
}

void VelocityComponent::setSpeed(f32 speed) {
    speed_ = speed;
}
