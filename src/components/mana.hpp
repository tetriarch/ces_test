#pragma once

#include "../component.hpp"

struct Mana {
    f32 current{0.0f};
    f32 max{0.0f};
    f32 regen{0.0f};
};

class ManaComponent : public Component<ManaComponent> {
public:
    ManaComponent();
    void attach() override;
    const Mana& mana() const;
    void setMana(const Mana& mana);
    void reset();
    void reduceMana(u32 amount);
    void increaseMana(u32 amount);

    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

private:
    void regen(const f32 dt);

private:
    Mana mana_;
};
