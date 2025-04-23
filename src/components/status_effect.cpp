#include "status_effect.hpp"

void StatusEffectComponent::addEffect(StatusEffect effect, EffectType type) {

    ActiveEffect e;
    e.effect = effect;
    e.type = type;
    e.currentDuration = 0.0f;

    effects_.push_back(e);
}

void StatusEffectComponent::update(const f32 dt) {

}

void StatusEffectComponent::postUpdate(const f32 dt) {

}