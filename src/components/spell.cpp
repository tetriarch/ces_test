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

void SpellComponent::update(f32 dt) {

    auto oldPosition = entity()->transform().position;
    for(auto&& action : spellData_->actions) {
        action.motion->apply(entity(), dt);
    }
    auto newPosition = entity()->transform().position;

    currentDuration_ += dt;
    traveledDistance_ += Vec2(newPosition - oldPosition).length();
}

void SpellComponent::postUpdate(f32 dt) {

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

    std::visit([&](auto& e) {
        return e.apply(applicant, target, rng);
    }, effect);
}

void DirectDamage::apply(EntityPtr owner, EntityPtr target, RandomNumberGenerator& rng) {

    auto targetLifeComponent = target->component<LifeComponent>();

    if(!targetLifeComponent) {
        return;
    }

    f32 dmg = rng.getFloat(min, max);
    targetLifeComponent->reduceLife(dmg);

    // just to see what's going on
    std::cout << "Damage " << dmg << " dealt to: " << target->name();
    std::cout << " [" << min << " - " << max << "]" << std::endl;
    std::cout << "Current life of " << target->name() << " is " << targetLifeComponent->life().current << std::endl;
}

void DamageOverTime::apply(EntityPtr owner, EntityPtr target, RandomNumberGenerator& rng) {

}

void Slow::apply(EntityPtr owner, EntityPtr target, RandomNumberGenerator& rng) {

}

void Stun::apply(EntityPtr owner, EntityPtr target, RandomNumberGenerator& rng) {

}

void Heal::apply(EntityPtr owner, EntityPtr target, RandomNumberGenerator& rng) {

}
