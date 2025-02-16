#include "damage.hpp"
#include "components.hpp"
#include "../math.hpp"


void Damage::setDamage(u32 min, u32 max) {
    minDamage_ = min;
    maxDamage_ = max;
}

void Damage::dealDamage(EntityPtr target) const {
    auto components = target->getComponents();
    for(auto& c : components) {
        if(c->getComponentType() == typeid(LifeMana)) {
            f32 t = (rand() % 101) / 100.0f;
            // calculate damage dealt
            u32 dmg = static_cast<u32>(lerp(minDamage_, maxDamage_, t));
            auto targetLifeManaComp = dynamic_cast<LifeMana*>(c.get());
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