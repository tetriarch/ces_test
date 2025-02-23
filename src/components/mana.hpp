#pragma once

#include "../component.hpp"

struct Mana {
    u32 current{0};
    u32 max{0};
};

class ManaComponent : public Component<ManaComponent> {

public:
    const Mana& getMana() const;
    void setMana(const Mana& mana);
    void update();
    auto describe() -> std::string override;

private:
    Mana mana_;

};