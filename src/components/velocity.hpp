#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../utils.hpp"

enum class MovementDirection : u8 {
    NONE = 0,
    NORTH = 1 << 0,
    EAST = 1 << 1,
    SOUTH = 1 << 2,
    WEST = 1 << 3
};

class VelocityComponent : public Component<VelocityComponent> {

public:
    auto describe() -> std::string override;
    void update() override;
    Vec2 velocity() const;
    f32 speed() const;
    void setMotion(MovementDirection direction);
    void stopMotion(MovementDirection direction);
    void setSpeed(f32 speed);

private:
    u8 movementDirection_{0};
    f32 speed_;
};