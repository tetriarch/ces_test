#include "status_effect.hpp"


void StatusEffectComponent::addBuff(Buff effect) {

    ActiveBuff buff;
    buff.effect = effect;
    buff.currentDuration = 0;

    buffs_.push_back(buff);
}

void StatusEffectComponent::addDebuff(Debuff effect) {

    ActiveDebuff debuff;
    debuff.effect = effect;
    debuff.currentDuration = 0;

    debuffs_.push_back(debuff);
}

void StatusEffectComponent::update(const f32 dt) {

}

void StatusEffectComponent::postUpdate(const f32 dt) {

}