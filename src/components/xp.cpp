#include "xp.hpp"

#include "../entity.hpp"
#include "../log.hpp"
#include "life.hpp"
#include "mana.hpp"

const u32 BASE_LEVEL_XP = 500;
const f32 LEVEL_XP_STEEPNESS = 1.5f;
const f32 LEVEL_XP_MULTIPLIER = 3.1f;
const u32 MAX_LEVEL = 99;

XPComponent::XPComponent()
    : currentXP_(0),
      nextLevelXP_(BASE_LEVEL_XP),
      previousLevelXP_(0),
      level_(1),
      progression_(false) {
}

void XPComponent::gainXP(u32 amount) {
    currentXP_ += amount;
}
void XPComponent::gainXP(u32 sourceLevel, u32 amount) {
    u32 levelDiff = level_ > sourceLevel ? level_ - sourceLevel : sourceLevel - level_;
    u32 xp = amount * sourceLevel * (1.0f - static_cast<f32>(levelDiff) * 0.1f);
    currentXP_ += xp;
}

void XPComponent::setLevel(u32 level) {
    level_ = level;
}

void XPComponent::setProgression(bool progression) {
    progression_ = progression;
}

u32 XPComponent::level() const {
    return level_;
}

u32 XPComponent::currentXP() const {
    return currentXP_;
}

u32 XPComponent::nextLevelXP() const {
    return nextLevelXP_;
}

void XPComponent::update(const f32 dt) {
    if(level_ == MAX_LEVEL || !progression_) {
        return;
    }

    while(currentXP_ >= nextLevelXP_ && level_ < MAX_LEVEL) {
        levelUp();
    }
}

void XPComponent::levelUp() {
    ++level_;
    auto lifeComponent = entity()->component<LifeComponent>();
    auto manaComponent = entity()->component<ManaComponent>();
    if(lifeComponent) {
        auto life = lifeComponent->life();
        life.max *= 1.1f;
        life.current = life.max;
        lifeComponent->setLife(life);
    }
    if(manaComponent) {
        auto mana = manaComponent->mana();
        mana.max *= 1.1f;
        mana.current = mana.max;
        manaComponent->setMana(mana);
    }
    INFO("[XP]: " + entity()->name() + " reached level " + std::to_string(level_));

    if(level_ == MAX_LEVEL) {
        currentXP_ = 0;
        nextLevelXP_ = 0;
        INFO("[XP]: " + entity()->name() + " reached max level");
    } else {
        currentXP_ = currentXP_ - nextLevelXP_;
        previousLevelXP_ = nextLevelXP_;
        nextLevelXP_ = previousLevelXP_ *
                       (1.0f + LEVEL_XP_STEEPNESS / (1.0f + (level_ - 1) / LEVEL_XP_MULTIPLIER));
        INFO("[XP]: next level requires " + std::to_string(nextLevelXP_) + " XP");
    }
}
