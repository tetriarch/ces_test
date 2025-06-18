#include "ai.hpp"

#include "../entity.hpp"
#include "../entity_manager.hpp"
#include "spell_book.hpp"
#include "status_effect.hpp"
#include "velocity.hpp"

AIComponent::AIComponent() : idleTimer_(0.0f), aggroRadius_(0.0f) {
}

void AIComponent::attach() {
    auto velocityComponent = entity()->component<VelocityComponent>();
    if(!velocityComponent) {
        ERROR(entity()->name() + "does not have velocity component");
        return;
    }
    velocityComponent->setAIControl(true);
}

void AIComponent::update(const f32 dt) {
    auto velocityComponent = entity()->component<VelocityComponent>();

    // no update if stunned
    auto statusEffectComponent = entity()->component<StatusEffectComponent>();
    if(statusEffectComponent) {
        if(statusEffectComponent->isUnderEffect(SpellEffectType::STUN)) {
            return;
        }
    }

    if(state_.mode == AIMode::IDLE) {
        auto transform = entity()->transform();

        if(idleTimer_ <= 0) {
            Vec2 direction;
            direction.x = rng_.getFloat(-1.0f, 1.0f);
            direction.y = rng_.getFloat(-1.0f, 1.0f);
            if(direction.length() >= 0.2f) {
                velocityComponent->setMotion({direction.x, direction.y});
                transform.rotationInDegrees = angleFromDirection(direction);
                entity()->setTransform(transform);
            } else {
                // clear motion on short direction length
                velocityComponent->clearMotion();
            }
            idleTimer_ = rng_.getFloat(0.5f, 1.5f);
        } else {
            idleTimer_ -= dt;
        }
        if(enemyInRange()) {
            state_.mode = AIMode::COMBAT;
        }
    }

    if(state_.mode == AIMode::COMBAT) {
        // pick a target
        u32 targetIndex = rng_.getUnsigned(0, enemiesInRange_.size() - 1);
        target_ = enemiesInRange_[targetIndex];
        // target direction
        auto transform = entity()->transform();
        auto target = target_.lock();

        auto spellBookComponent = entity()->component<SpellBookComponent>();

        if(target) {
            auto targetTransform = target->transform();

            Vec2 targetDirection = targetTransform.position - transform.position;

            // update motion
            velocityComponent->setMotion(targetDirection);
            transform.rotationInDegrees = angleFromDirection(targetDirection);
            entity()->setTransform(transform);

            if(spellBookComponent) {
                u32 spellIndex = rng_.getUnsigned(0, spellBookComponent->spells().size() - 1);
                if(!spellBookComponent->isCasting()) {
                    spellBookComponent->castSpell(spellIndex, targetDirection);
                }
            }
        }
        if(!target) {
            if(!enemyInRange()) {
                state_.mode = AIMode::IDLE;
                idleTimer_ = 0;
                if(spellBookComponent->isCasting()) {
                    spellBookComponent->interruptCasting();
                }
            }
        }
    }
}

void AIComponent::setAggroRadius(f32 radius) {
    aggroRadius_ = radius;
}

bool AIComponent::enemyInRange() const {
    enemiesInRange_.clear();
    auto entities = EntityManager::get().entities();
    for(auto& e : entities) {
        auto ePtr = e.second.lock();
        if(!ePtr || ePtr == entity()) {
            continue;
        }
        // no tag, skip
        auto eTagComponent = ePtr->component<TagComponent>();
        auto tagComponent = entity()->component<TagComponent>();
        if(!eTagComponent) {
            continue;
        }
        if(!tagComponent) {
            ERROR_ONCE("[AI]: missing tag component");
            return false;
        }
        if(tagComponent->isHostile(eTagComponent->tag())) {
            auto transform = entity()->transform();
            auto eTransform = ePtr->transform();

            f32 distance = Vec2(eTransform.position - transform.position).length();
            if(distance < aggroRadius_) {
                enemiesInRange_.emplace_back(ePtr);
            }
            if(!enemiesInRange_.empty()) {
                return true;
            }
        }
    }
    return false;
}
