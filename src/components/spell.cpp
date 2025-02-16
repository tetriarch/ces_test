#include "spell.hpp"

Spell::Spell(std::string name, f32 castTime, f32 interruptTime, u32 manaCost, f32 cooldown) :
    name_(name),
    castTime_(castTime),
    interruptTime_(interruptTime),
    manaCost_(manaCost),
    cooldown_(cooldown) {}

void Spell::addAction(const SpellAction& action) {
    actions_.emplace_back(action);
}

auto Spell::describe() -> std::string {
    return "Name: " + name_ + "\n -" +
        " Cast Time: " + std::to_string(castTime_) +
        " Interrupt Time: " + std::to_string(interruptTime_) +
        " Mana Cost: " + std::to_string(manaCost_) +
        " Cooldown:" + std::to_string(cooldown_);
}
