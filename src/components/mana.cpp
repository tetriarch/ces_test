#include "mana.hpp"

const Mana& ManaComponent::mana() const {
    return mana_;
}

void ManaComponent::setMana(const Mana& mana) {
    mana_ = mana;
}
