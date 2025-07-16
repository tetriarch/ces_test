#include "status_effect.hpp"

#include "../asset_manager.hpp"
#include "../entity.hpp"
#include "../log.hpp"
#include "ai.hpp"
#include "animation.hpp"
#include "life.hpp"
#include "spell.hpp"
#include "visual_status_effect.hpp"

void StatusEffectComponent::applyEffect(const SpellEffect& effect) {
    // handle direct effects
    if(effect.isDirect()) {
        applyDirectEffect(effect);
        return;
    }
    auto aiComponent = entity()->component<AIComponent>();
    if(aiComponent) {
        aiComponent->resolveEffectApplier(effect.applier.lock());
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

    if(e.visual) {
        auto am = AssetManager::get();
        auto visualEffect = am->load<StatusEffectData>(e.effectFilePath);
        if(!visualEffect) {
            ERROR("[STATUS EFFECT]: " + e.name +
                  " is flagged as visual but no visual data has been found");
            return;
        }

        auto effectSpawn = Entity::create(e.name);
        if(visualEffect->animated) {
            auto animationComponent = std::make_shared<AnimationComponent>();
            animationComponent->addAnimationFiles(visualEffect->animationFiles);
            animationComponent->queueAnimation("idle");
            effectSpawn->addComponent(animationComponent);
        }

        auto visualStatusEffectComponent = std::make_shared<VisualStatusEffectComponent>();
        visualStatusEffectComponent->setTextureFile(visualEffect->textureFilePath);
        visualStatusEffectComponent->setRect(visualEffect->rect);
        effectSpawn->addComponent(visualStatusEffectComponent);

        entity()->addChild(effectSpawn);
    }
}

void StatusEffectComponent::applyDirectEffect(const SpellEffect& effect) {
    auto lifeComponent = entity()->component<LifeComponent>();
    auto value = rng_.getUnsigned(effect.minValue, effect.maxValue);

    if(lifeComponent) {
        if(effect.type == SpellEffectType::DIRECT_DAMAGE) {
            lifeComponent->reduceLife(value, effect.applier.lock());
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

        // remove visual effect
        if(it->second.visual) {
            auto children = entity()->children();
            for(auto& c : children) {
                if(c->name() == it->second.name) {
                    entity()->removeChild(c);
                    break;
                }
            }
        }
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
            lifeComponent->reduceLife(dmg, effect.applier.lock());
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
