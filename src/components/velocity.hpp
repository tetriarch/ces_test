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
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;
    Vec2 velocity() const;
    f32 speed() const;
    void setMotion(MovementDirection direction);
    void stopMotion(MovementDirection direction);
    void setSpeed(f32 speed);

private:
    Transform lastTransform_;
    u8 movementDirection_{0};
    f32 speed_;
};