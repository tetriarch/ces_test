#include "reward.hpp"

RewardComponent::RewardComponent() : xp_(0) {
}

void RewardComponent::setXP(u32 xp) {
    xp_ = xp;
}

u32 RewardComponent::xp() const {
    return xp_;
}
