#include "velocity.hpp"
#include "../entity.hpp"

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
    return velocityVector.normalized() * speed_;
}

void VelocityComponent::update() {

    auto transform = entity()->transform();
    transform.position += velocity();
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
