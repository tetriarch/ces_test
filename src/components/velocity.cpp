#include "velocity.hpp"
#include "../entity.hpp"

auto VelocityComponent::describe() -> std::string {
    return "I move";
}

Vec2 VelocityComponent::velocity() const {
    return direction_.normalized() * speed_;
}

void VelocityComponent::update() {

    auto transform = entity()->transform();
    std::string name = entity()->name();
    transform.position += velocity();
    entity()->setTransform(transform);
}

Vec2 VelocityComponent::direction() const {
    return direction_;
}

f32 VelocityComponent::speed() const {
    return speed_;
}

void VelocityComponent::setDirection(const Vec2& direction) {
    direction_ += direction;
}

void VelocityComponent::setSpeed(f32 speed) {
    speed_ = speed;
}
