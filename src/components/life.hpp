#pragma once

#include "../component.hpp"

struct Life {
    f32 current{0.0f};
    f32 max{0.0f};
    f32 regen{0.0f};
};

class LifeComponent : public Component<LifeComponent> {
public:
    LifeComponent();
    void attach() override;
    const Life& life() const;
    void reset();
    void setLife(const Life& life);
    void reduceLife(f32 amount, EntityPtr applier);
    void increaseLife(f32 amount);
    bool isAtFull();
    bool isDead() const;
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;
    void render(std::shared_ptr<Renderer> renderer) override;

private:
    void regen(const f32 dt);

private:
    Life life_;
    bool dead_;
    EntityHandle lastAttacker_;
};
