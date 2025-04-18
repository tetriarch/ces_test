#include "spell.hpp"
#include "components.hpp"

#include "../random_number_generator.hpp"

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData) {

    spellData_ = spellData;
}

void SpellComponent::attach() {

    currentDuration_ = 0.0f;
    traveledDistance_ = 0.0f;
    dead_ = false;
}

void SpellComponent::update(const f32 dt) {

    auto oldPosition = entity()->transform().position;
    for(auto&& action : spellData_->actions) {
        action.motion->apply(entity(), dt);
    }
    auto newPosition = entity()->transform().position;

    currentDuration_ += dt;
    traveledDistance_ += Vec2(newPosition - oldPosition).length();
}

void SpellComponent::postUpdate(const f32 dt) {

    if(dead_) {
        entity()->parent()->queueRemoveChild(entity());
        return;
    }

    if(spellData_->duration != 0) {
        if(currentDuration_ >= spellData_->duration) {
            dead_ = true;
        }
    }
    else if(traveledDistance_ >= spellData_->maxRange) {
        dead_ = true;
    }

    auto collisionComponent = entity()->component<CollisionComponent>();
    auto ownerComponent = entity()->component<OwnerComponent>();

    if(collisionComponent && ownerComponent) {
        if(collisionComponent->collided()) {

            auto target = collisionComponent->collisionSource();
            bool effectApllied = false;

            for(auto& a : spellData_->actions) {
                for(auto& onHitEffect : a.effects) {

                    if(!canApplyEffect(ownerComponent->owner(), target, onHitEffect)) {
                        continue;
                    }
                    applyEffect(ownerComponent->owner(), target, onHitEffect.effect, rng_);
                    effectApllied = true;
                }
            }

            if(effectApllied) {
                dead_ = true;
            }
        }
    }
}

bool SpellComponent::canApplyEffect(EntityPtr applicant, EntityPtr target, SpellEffectOnHit onHitEffect) {

    auto applicantTagComponent = applicant->component<TagComponent>();
    auto targetTagComponent = target->component<TagComponent>();

    if(!applicantTagComponent || !targetTagComponent) {
        return false;
    }

    auto targetTag = targetTagComponent->tag();

    bool belongs;

    if(onHitEffect.targetFaction == FactionType::FRIENDLY) {
        belongs = applicantTagComponent->isFriendly(targetTag);
    }
    else if(onHitEffect.targetFaction == FactionType::HOSTILE) {
        belongs = applicantTagComponent->isHostile(targetTag);
    }
    else {
        // unknown faction
        belongs = false;
    }

    return belongs;
}

void SpellComponent::applyEffect(EntityPtr applicant, EntityPtr target, SpellEffect effect, RandomNumberGenerator& rng) {

    std::visit([&](const auto& e) {
        return applyEffect(target, e);
    }, effect);
}

void SpellComponent::applyEffect(EntityPtr target, DirectDamage damage) {

    auto lifeComponent = target->component<LifeComponent>();

    if(!lifeComponent) {
        return;
    }

    f32 amount = rng_.getFloat(damage.min, damage.max);
    lifeComponent->reduceLife(amount);
}

void SpellComponent::applyEffect(EntityPtr target, DamageOverTime dot) {

    auto statusEffectComponent = target->component<StatusEffectComponent>();

    if(!statusEffectComponent) {
        return;
    }

    statusEffectComponent->addDebuff(dot);
}

void SpellComponent::applyEffect(EntityPtr target, Slow slow) {

    auto statusEffectComponent = target->component<StatusEffectComponent>();

    if(!statusEffectComponent) {
        return;
    }

    statusEffectComponent->addDebuff(slow);
}

void SpellComponent::applyEffect(EntityPtr target, Haste haste) {

    auto statusEffectComponent = target->component<StatusEffectComponent>();

    if(!statusEffectComponent) {
        return;
    }

    statusEffectComponent->addBuff(haste);
}

void SpellComponent::applyEffect(EntityPtr target, Stun stun) {

    auto statusEffectComponent = target->component<StatusEffectComponent>();

    if(!statusEffectComponent) {
        return;
    }

    statusEffectComponent->addDebuff(stun);
}

void SpellComponent::applyEffect(EntityPtr target, Heal heal) {

    auto targetLifeComponent = target->component<LifeComponent>();

    if(!targetLifeComponent) {
        return;
    }

    f32 amount = rng_.getFloat(heal.min, heal.max);
    targetLifeComponent->increaseLife(amount);
}

void SpellComponent::applyEffect(EntityPtr target, HealOverTime hot) {

    auto statusEffectComponent = target->component<StatusEffectComponent>();

    if(!statusEffectComponent) {
        return;
    }

    statusEffectComponent->addBuff(hot);
}