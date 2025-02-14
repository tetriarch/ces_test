#pragma once

#include "../component.hpp"
#include "../entity.hpp"

class DamageComponent : public Component<DamageComponent> {

public:
    void setDamage(u32 min, u32 max);
    void dealDamage(EntityPtr target) const;



private:
    u32 minDamage_{0};
    u32 maxDamage_{0};
};