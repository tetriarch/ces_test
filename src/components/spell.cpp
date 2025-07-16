#include "spell.hpp"

#include "animation.hpp"
#include "owner.hpp"
#include "particle_system.hpp"
#include "spawn.hpp"
#include "status_effect.hpp"

bool SpellEffect::isDirect() const {
    return (type == SpellEffectType::DIRECT_DAMAGE || type == SpellEffectType::DIRECT_HEAL);
}

bool SpellData::requiresComponent(SpellRequirement req) const {
    return (componentRequirements & static_cast<u8>(req)) != 0;
}

SpellComponent::SpellComponent() : casterTag_(TagType::UNKNOWN) {
}

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData)
    : spellData_(spellData), casterTag_(TagType::UNKNOWN) {
}

void SpellComponent::attach() {
    currentDuration_ = 0.0f;
    traveledDistance_ = 0.0f;
    dead_ = false;
}

void SpellComponent::update(const f32 dt) {
    if(dead_) {
        return;
    }

    auto oldPosition = entity()->transform().position;

    if(spellData_->action.type != ActionType::SELF &&
        spellData_->action.type != ActionType::SPAWN) {
        spellData_->action.motion->apply(entity(), dt);
        auto newPosition = entity()->transform().position;
        traveledDistance_ += Vec2(newPosition - oldPosition).length();
    }

    if(spellData_->action.type == ActionType::SPAWN) {
        auto spawnComponent = entity()->component<SpawnComponent>();
        if(spawnComponent) {
            spawnComponent->spawn(oldPosition);
            dead_ = true;
        }
    }
    currentDuration_ += dt;
}

void SpellComponent::postUpdate(const f32 dt) {
    auto animationComponent = entity()->component<AnimationComponent>();

    if(dead_) {
        if(!animationComponent) {
            entity()->parent()->removeChild(entity());
        }
        return;
    }

    if(spellData_->action.type != ActionType::SELF) {
        if(spellData_->duration > 0) {
            if(currentDuration_ >= spellData_->duration) {
                dead_ = true;
            }
        } else if(traveledDistance_ >= spellData_->maxRange) {
            dead_ = true;
        }
    }

    auto ownerComponent = entity()->component<OwnerComponent>();
    if(spellData_->action.type == ActionType::SELF) {
        for(auto& effect : spellData_->action.effects) {
            if(ownerComponent) {
                auto owner = ownerComponent->owner().lock();
                auto statusEffectComponent = owner->component<StatusEffectComponent>();
                if(statusEffectComponent) {
                    effect.applier = owner;
                    statusEffectComponent->applyEffect(effect);
                    dead_ = true;
                }
            }
        }
    } else {
        auto collisionComponent = entity()->component<CollisionComponent>();

        if(collisionComponent && ownerComponent) {
            if(collisionComponent->collided()) {
                auto colliders = collisionComponent->colliders();
                bool effectApplied = false;

                for(auto target : colliders) {
                    for(auto& effect : spellData_->action.effects) {
                        if(canApplyEffect(target, effect)) {
                            auto statusEffectComponent = target->component<StatusEffectComponent>();

                            if(statusEffectComponent) {
                                effect.applier = ownerComponent->owner();
                                statusEffectComponent->applyEffect(effect);
                                effectApplied = true;
                            }
                        }
                    }
                }
                if(effectApplied) {
                    dead_ = true;
                }
            }
        }
    }

    if(dead_) {
        // play on death animation if there is any
        auto animation = entity()->component<AnimationComponent>();
        if(animation) {
            animation->playAnimation(
                "death", [&]() { return entity()->parent()->removeChild(entity()); });
        }

        auto particles = entity()->component<ParticleSystemComponent>();
        if(particles) {
            particles->setEmitting(false);
        }
    }
}

bool SpellComponent::isDead() {
    return dead_;
}

void SpellComponent::setCasterTag(TagType tag) {
    casterTag_ = tag;
}

bool SpellComponent::canApplyEffect(EntityPtr target, SpellEffect effect) {
    auto targetTagComponent = target->component<TagComponent>();

    if(!targetTagComponent) {
        return false;
    }

    bool belongs;

    if(effect.targetFaction == FactionType::FRIENDLY) {
        belongs = targetTagComponent->isFriendly(casterTag_);
    } else if(effect.targetFaction == FactionType::HOSTILE) {
        belongs = targetTagComponent->isHostile(casterTag_);
    } else {
        // unknown faction
        belongs = false;
    }

    return belongs;
}
