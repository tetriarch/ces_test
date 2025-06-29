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
    u32 xp = 0;
    if(levelDiff <= 10) {
        xp = amount * sourceLevel * (1.0f - static_cast<f32>(levelDiff) * 0.1f);
    }
    currentXP_ += xp;
}

void XPComponent::setLevel(u32 level) {
    level_ = std::min(level, MAX_LEVEL);
    resetXP();
    if(entity()) {
        auto lifeComponent = entity()->component<LifeComponent>();
        auto manaComponent = entity()->component<ManaComponent>();
        if(lifeComponent) {
            lifeComponent->reset();
        }
        if(manaComponent) {
            manaComponent->reset();
        }
    }
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

bool XPComponent::isMaxLevel() const {
    return level_ == MAX_LEVEL;
}

void XPComponent::update(const f32 dt) {
    if(level_ == MAX_LEVEL || !progression_) {
        return;
    }

    while(currentXP_ >= nextLevelXP_ && level_ < MAX_LEVEL) {
        levelUp();
        auto lifeComponent = entity()->component<LifeComponent>();
        auto manaComponent = entity()->component<ManaComponent>();
        if(lifeComponent) {
            lifeComponent->reset();
        }
        if(manaComponent) {
            manaComponent->reset();
        }
    }
}

void XPComponent::levelUp() {
    ++level_;
    INFO("[XP]: you have reached level " + std::to_string(level_));

    if(level_ == MAX_LEVEL) {
        currentXP_ = 0;
        nextLevelXP_ = 0;
        INFO("[XP]: you have reached maximum level");
    } else {
        currentXP_ = currentXP_ - nextLevelXP_;
        previousLevelXP_ = nextLevelXP_;
        f32 xp = previousLevelXP_ *
                 (1.0f + LEVEL_XP_STEEPNESS / (1.0f + (level_ - 1) / LEVEL_XP_MULTIPLIER));

        nextLevelXP_ = xp;
        INFO("[XP]: next level requires " + std::to_string(nextLevelXP_) + " XP");
    }
}

void XPComponent::resetXP() {
    if(level_ == MAX_LEVEL) {
        previousLevelXP_ = 0;
        nextLevelXP_ = 0;
        currentXP_ = 0;
        return;
    }

    u32 i = 1;
    previousLevelXP_ = 0;
    currentXP_ = 0;
    nextLevelXP_ = BASE_LEVEL_XP;
    while(i < level_) {
        previousLevelXP_ = nextLevelXP_;
        f32 xp =
            previousLevelXP_ * (1.0f + LEVEL_XP_STEEPNESS / (1.0f + (i) / LEVEL_XP_MULTIPLIER));

        nextLevelXP_ = xp;
        i++;
    }
}
