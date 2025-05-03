#include "status_effect.hpp"
#include "life.hpp"
#include "../entity.hpp"
#include "../log.hpp"

#include "spell.hpp"

void StatusEffectComponent::addEffect(const SpellEffect& effect) {

    //TODO: Find out whether effect is already in palce, if so check the statck count, refresh duration
    //: If The the effect is not in collection, add it

    ActiveEffect e;
    e.effect = effect;
    e.currentDuration = 0.0f;

    if(e.stacks < effect.maxStacks) {
        e.stacks += 1;
    }

    effects_.push_back(e);
}


bool StatusEffectComponent::removeEffect(ActiveEffect* effect) {

    for(u32 i = 0; i < effects_.size(); i++) {

        if(effects_[i].effect.name == effect->effect.name) {
            INFO("[STATUS EFFECT]: Removed " + effect->effect.name + " after " + std::to_string(effects_[i].currentDuration) + "s");
            effects_.erase(effects_.begin() + i);
            return true;
        }
    }
    return false;
}

void StatusEffectComponent::update(const f32 dt) {

    for(auto& e : effects_) {

        updateEffect(e, dt);

        if(e.currentDuration >= e.effect.duration) {
            effectsToRemove_.push_back(&e);
        }
    }
}

void StatusEffectComponent::postUpdate(const f32 dt) {

    // remove effects 
    while(!effectsToRemove_.empty()) {
        if(removeEffect(effectsToRemove_.back())) {
            effectsToRemove_.pop_back();
        }
    }
}

bool StatusEffectComponent::isUnderEffect(SpellEffectType type) const {

    for(auto& e : effects_) {
        if(e.effect.effectType == type) {
            return true;
        }
    }

    return false;
}

void StatusEffectComponent::updateEffect(ActiveEffect& effect, const f32 dt) {


    auto lifeComponent = entity()->component<LifeComponent>();

    //TODO: Sort this naming shit out - it's straight up disgusting

    auto effectType = effect.effect.effectType;

    switch(effectType) {
        case SpellEffectType::DIRECT_DAMAGE: {
            if(lifeComponent) {
                u32 dmg = rng_.getUnsigned(effect.effect.minValue, effect.effect.maxValue);
                lifeComponent->reduceLife(dmg);
            }
            break;
        }

        case SpellEffectType::DAMAGE_OVER_TIME: {
            if(lifeComponent) {
                f32 dmg = effect.effect.periodicValue * dt;
                lifeComponent->reduceLife(dmg);
            }
            break;
        }

        case SpellEffectType::SLOW: {
            //TODO: StatModifierComponent
            break;
        }
        case SpellEffectType::HASTE: {
            //TODO: StatModifierComponent
            break;
        }
        case SpellEffectType::STUN: {
            //TODO: StatModifierComponent
            break;
        }
        case SpellEffectType::HEAL: {
            if(lifeComponent) {
                u32 heal = rng_.getUnsigned(effect.effect.minValue, effect.effect.maxValue);
                lifeComponent->increaseLife(heal);
            }
            break;
        }
        case SpellEffectType::HEAL_OVER_TIME: {
            f32 heal = effect.effect.periodicValue * dt;
            lifeComponent->increaseLife(heal);
            break;
        }
        default:
            break;
    }

    effect.currentDuration += dt;
}