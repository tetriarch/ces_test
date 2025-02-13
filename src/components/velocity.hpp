#pragma once

#include "../component.hpp"
#include "../math.hpp"

class VelocityComponent : public Component<VelocityComponent> {

public:
    Vec2 getVelocity() const {
        return direction_.normalized() * speed_;
    }
    Vec2 getDirection() const { return direction_; }
    f32 getSpeed() const { return speed_; }
    void setDirection(const Vec2& direction) { direction_ = direction; }
    void setSpeed(f32 speed) { speed_ = speed; }

private:
    Vec2 direction_;
    f32 speed_;
};