#pragma once

#include "../component.hpp"
#include "../math.hpp"

class VelocityComponent : public Component<VelocityComponent> {

public:
    auto describe() -> std::string override;
    void update() override;
    Vec2 velocity() const;
    Vec2 direction() const;
    f32 speed() const;
    void setDirection(const Vec2& direction);
    void setSpeed(f32 speed);

private:
    Vec2 direction_{0,0};
    f32 speed_;
};