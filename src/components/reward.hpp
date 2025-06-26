#pragma once

#include "../component.hpp"

class RewardComponent : public Component<RewardComponent> {
public:
    RewardComponent();
    void setXP(u32 xp);
    u32 xp() const;

private:
    u32 xp_;
};
