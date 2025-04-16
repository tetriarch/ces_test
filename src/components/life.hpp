#pragma once

#include "../component.hpp"

struct Life {
    f32 current{0.0f};
    f32 max{0.0f};
    f32 regen{0.0f};
};

class LifeComponent : public Component<LifeComponent> {

public:
    void attach() override;
    const Life& life() const;
    void setLife(const Life& life);
    void reduceLife(f32 amount);
    void increaseLife(f32 amount);

    void update(f32 dt) override;
    void postUpdate(f32 dt) override;

private:
    void regen(f32 dt);

private:
    Life life_;
    bool dead;
};