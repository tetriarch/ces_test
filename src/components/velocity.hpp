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
    s32 updatePriority() override {
        return 2;
    }
    void attach() override;
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;
    Vec2 velocity() const;
    f32 speed() const;
    void setAIControl(bool aiControl);
    void clearMotion();
    void setMotion(MovementDirection direction);
    void setMotion(const Vec2& direction);
    void stopMotion(MovementDirection direction);
    void setSpeed(f32 speed);

private:
    Vec2 movementDirectionToVector() const;

private:
    Transform lastTransform_;
    u8 movementDirection_{0};
    Vec2 motionVector_ = {0.0f, 0.0f};
    f32 speed_;
    bool aiControled_{false};
    size_t colliderListenerId_;
};
