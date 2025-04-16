#pragma once

#include "../component.hpp"

struct Mana {
    f32 current{0.0f};
    f32 max{0.0f};
    f32 regen{0.0f};
};

class ManaComponent : public Component<ManaComponent> {

public:
    const Mana& mana() const;
    void setMana(const Mana& mana);
    void reduceMana(u32 amount);
    void increaseMana(u32 amount);

    void update(f32 dt) override;
    void postUpdate(f32 dt) override;

private:
    void regen(f32 dt);

private:
    Mana mana_;
};