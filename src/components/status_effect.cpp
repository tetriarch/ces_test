#include "status_effect.hpp"

#include "../entity.hpp"
#include "../log.hpp"
#include "life.hpp"
#include "spell.hpp"

void StatusEffectComponent::applyEffect(const SpellEffect& effect) {
    // handle direct effects
    if(effect.isDirect()) {
        applyDirectEffect(effect);
        return;
    }

    // check if is the effect already present
    if(auto it = effects_.find(effect.type); it != effects_.end()) {
        auto& e = it->second;
        // check if we can add stacks
        if(e.currentStacks < e.maxStacks) {
            e.currentStacks++;
        }

        // reset duration
        it->second.currentDuration = 0.0f;
        return;
    }

    SpellEffect e = effect;

    e.currentDuration = 0.0f;
    e.currentStacks = 1;
    effects_.insert(std::pair<SpellEffectType, SpellEffect>(e.type, e));
}

void StatusEffectComponent::applyDirectEffect(const SpellEffect& effect) {
    auto lifeComponent = entity()->component<LifeComponent>();
    auto value = rng_.getUnsigned(effect.minValue, effect.maxValue);

    if(lifeComponent) {
        if(effect.type == SpellEffectType::DIRECT_DAMAGE) {
            lifeComponent->reduceLife(value);
        }
        if(effect.type == SpellEffectType::DIRECT_HEAL) {
            lifeComponent->increaseLife(value);
        }
    }
}

bool StatusEffectComponent::removeEffect(SpellEffectType type) {
    if(auto it = effects_.find(type); it != effects_.end()) {
        INFO("[STATUS EFFECT]: Removed " + it->second.name + " after " +
             std::to_string(it->second.currentDuration) + "s");
        effects_.erase(it);
        return true;
    }

    return false;
}

auto StatusEffectComponent::effect(SpellEffectType type) const -> const SpellEffect* {
    if(auto it = effects_.find(type); it != effects_.end()) {
        return &it->second;
    }
    return nullptr;
}

void StatusEffectComponent::update(const f32 dt) {
    for(auto& [type, effect] : effects_) {
        updateEffect(effect, dt);

        if(effect.currentDuration >= effect.maxDuration) {
            effectsToRemove_.emplace_back(type);
        }
    }
}

void StatusEffectComponent::postUpdate(const f32 dt) {
    while(!effectsToRemove_.empty()) {
        if(removeEffect(effectsToRemove_.back())) {
            effectsToRemove_.pop_back();
        }
    }
}

bool StatusEffectComponent::isUnderEffect(SpellEffectType type) const {
    return effects_.find(type) != effects_.end();
}

void StatusEffectComponent::updateEffect(SpellEffect& effect, const f32 dt) {
    auto lifeComponent = entity()->component<LifeComponent>();
    auto effectType = effect.type;

    if(effectType == SpellEffectType::DAMAGE_OVER_TIME) {
        if(lifeComponent) {
            f32 dmg = effect.periodicValue * effect.currentStacks * dt;
            lifeComponent->reduceLife(dmg);
        }
    }

    if(effectType == SpellEffectType::HEAL_OVER_TIME) {
        if(lifeComponent) {
            f32 heal = effect.periodicValue * effect.currentStacks * dt;
            lifeComponent->increaseLife(heal);
        }
    }

    effect.currentDuration += dt;
}
