#include "velocity.hpp"

auto VelocityComponent::describe() -> std::string {
    return "I move";
}

Vec2 VelocityComponent::velocity() const {
    return direction_.normalized() * speed_;
}

Vec2 VelocityComponent::direction() const {
    return direction_;
}

f32 VelocityComponent::speed() const {
    return speed_;
}

void VelocityComponent::setDirection(const Vec2& direction) {
    direction_ = direction;
}

void VelocityComponent::setSpeed(f32 speed) {
    speed_ = speed;
}
