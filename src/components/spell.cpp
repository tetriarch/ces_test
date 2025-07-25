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
    : spellData_(std::move(spellData)), casterTag_(TagType::UNKNOWN) {
}

void SpellComponent::attach() {
    if(spellData_->action.type != ActionType::SELF) {
        auto ownerComponent = entity()->component<OwnerComponent>();
        if (auto colComp = entity()->component<CollisionComponent>(); colComp && ownerComponent) {
            // Owner should be configured by now, so get the owner of this spell and pass it into
            // the collision callback
            auto owner = ownerComponent->owner();

            // We're capturing `this`, which can be dangerous since that's not a shared_ptr or a weak_ptr
            // Thankfully, we should never be processing collisions while destroying the game object.
            colliderListenerId_ = colComp->onCollision.subscribe([this, owner](EntityPtr const& target, auto normal, auto depth) {
                // Ignore collisions if we're in the wrong state
                if (this->state_ != State::Alive) return;

                for(auto& effect : spellData_->action.effects) {
                    if(canApplyEffect(target, effect)) {
                        if(auto statusEffectComponent = target->component<StatusEffectComponent>()) {
                            effect.applier = owner;
                            statusEffectComponent->applyEffect(effect);
                            this->state_ = State::Dying;
                        }
                    }
                }
            });
        }
    }
}

void SpellComponent::update(f32 dt) {
    if(state_ != State::Alive) {
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
            state_ = State::Dying;
        }
    }
    currentDuration_ += dt;
}

void SpellComponent::postUpdate(f32 dt) {
    if (state_ == State::Dead) return;

    if(state_ == State::Dying) {
        state_ = State::Dead;
        if(auto animation = entity()->component<AnimationComponent>()) {
            animation->playAnimation(
                "death", [&]() { return entity()->parent()->removeChild(entity()); });
        } else {
            entity()->parent()->removeChild(entity());
        }

        if (auto particles = entity()->component<ParticleSystemComponent>()) {
            particles->setEmitting(false);
        }
        return;
    }

    if(spellData_->action.type != ActionType::SELF) {
        if(spellData_->duration > 0) {
            if(currentDuration_ >= spellData_->duration) {
                state_ = State::Dying;
            }
        } else if(traveledDistance_ >= spellData_->maxRange) {
            state_ = State::Dying;
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
                    state_ = State::Dying;
                }
            }
        }
    }
}

void SpellComponent::setCasterTag(TagType tag) {
    casterTag_ = tag;
}

bool SpellComponent::canApplyEffect(EntityPtr target, SpellEffect effect) const {
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
