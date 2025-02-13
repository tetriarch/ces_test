#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"
#include "life_mana.hpp"


class DamageComponent : public Component<DamageComponent> {

public:
    void setDamage(u32 min, u32 max) { minDamage_ = min; maxDamage_ = max; }
    void dealDamage(EntityPtr target) const {
        auto components = target->getComponents();
        for(auto& c : components) {
            if(c->getComponentType() == typeid(LifeManaComponent)) {
                f32 t = (rand() % 101) / 100.0f;
                // calculate damage dealt
                u32 dmg = static_cast<u32>(lerp(minDamage_, maxDamage_, t));
                auto targetLifeManaComp = dynamic_cast<LifeManaComponent*>(c.get());
                u32 life = targetLifeManaComp->getLife();
                if(life < dmg) {
                    targetLifeManaComp->setLife(0);
                }
                else {
                    targetLifeManaComp->setLife(life - dmg);
                }
            }
        }
    }


private:
    u32 minDamage_{0};
    u32 maxDamage_{0};
};