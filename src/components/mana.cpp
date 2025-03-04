#include "mana.hpp"

const Mana& ManaComponent::mana() const {
    return mana_;
}

void ManaComponent::setMana(const Mana& mana) {
    mana_ = mana;
}

void ManaComponent::update() {

}

auto ManaComponent::describe() -> std::string {
    return "I have " + std::to_string(mana_.current) + "/" + std::to_string(mana_.max) + " mana";
}
