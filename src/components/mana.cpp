#include "mana.hpp"

const Mana& ManaComponent::mana() const {
    return mana_;
}

void ManaComponent::setMana(const Mana& mana) {
    mana_ = mana;
}

void ManaComponent::reduceMana(u32 amount) {
    mana_.current -= std::clamp(static_cast<f32>(amount), 0.0f, mana_.current);
}

void ManaComponent::increaseMana(u32 amount) {
    mana_.current = std::clamp(amount + mana_.current, mana_.current, mana_.max);
}

void ManaComponent::update(const f32 dt) {

    if(mana_.current < mana_.max) {
        regen(dt);
    }
}

void ManaComponent::postUpdate(const f32 dt) {

    // clamp
    if(mana_.current > mana_.max) {
        mana_.current = mana_.max;
    }
}

void ManaComponent::regen(const f32 dt) {

    mana_.current += mana_.regen * dt;
}