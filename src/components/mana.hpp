#pragma once

#include "../component.hpp"

struct Mana {
    u32 current{0};
    u32 max{0};
};

class ManaComponent : public Component<ManaComponent> {

public:
    const Mana& mana() const;
    void setMana(const Mana& mana);

private:
    Mana mana_;

};