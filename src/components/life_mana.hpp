#pragma once

#include "../component.hpp"

class LifeMana : public Component<LifeMana> {
public:
    auto describe() -> std::string override {
        return "I have " + std::to_string(life_) + " life and " + std::to_string(mana_) + " mana";
    }
    u32 getLife() const { return life_; }
    u32 getMana() const { return mana_; }
    void setLife(u32 life) { life_ = life; }
    void setMana(u32 mana) { mana_ = mana; }
private:
    u32 life_{0};
    u32 mana_{0};
};