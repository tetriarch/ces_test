#include "velocity.hpp"

#include "../entity.hpp"
#include "ai.hpp"
#include "collision.hpp"
#include "owner.hpp"
#include "spell_book.hpp"
#include "status_effect.hpp"

const f32 ON_CAST_MOVEMENT_SPEED_MULTIPLIER = 0.75f;

void VelocityComponent::attach() {
    if(auto colComp = entity()->component<CollisionComponent>(); colComp) {
        colliderListenerId_ = colComp->addOnCollisionListener(
            [entity = entity()](const EntityPtr& target, auto normal, auto depth) {
                // collision with spell -- skip
                if(auto spellComp = target->component<SpellComponent>(); spellComp) {
                    return;
                }

                auto targetOwnerComp = target->component<OwnerComponent>();
                if(!targetOwnerComp || (targetOwnerComp && !targetOwnerComp->isOwnedBy(entity))) {
                    auto statusEffectComp = entity->component<StatusEffectComponent>();
                    if(statusEffectComp &&
                       !statusEffectComp->isUnderEffect(SpellEffectType::STUN)) {
                        auto transform = entity->transform();
                        transform.position -= normal * depth;
                        entity->setTransform(transform);
                    }
                }
            });
    }
}

Vec2 VelocityComponent::velocity() const {
    Vec2 velocityVector = {0, 0};
    velocityVector = motionVector_;
    return velocityVector.normalized();
}

void VelocityComponent::update(const f32 dt) {
    f32 speed = speed_;
    f32 speedModifier = 1.0f;

    // slow down on casting
    auto spellBook = entity()->component<SpellBookComponent>();
    if(spellBook && spellBook->isCasting()) {
        speedModifier *= ON_CAST_MOVEMENT_SPEED_MULTIPLIER;
    }

    // modify speed when hasted or slowed
    auto statusEffect = entity()->component<StatusEffectComponent>();
    if(statusEffect) {
        auto haste = statusEffect->effect(SpellEffectType::HASTE);
        auto slow = statusEffect->effect(SpellEffectType::SLOW);
        auto stun = statusEffect->effect(SpellEffectType::STUN);

        if(haste) {
            speedModifier *= (1.0f + haste->magnitude);
        }
        if(slow) {
            speedModifier *= (1.0f - slow->magnitude);
        }
        if(stun) {
            speedModifier = 0.0f;
        }
    }
    auto aiComponent = entity()->component<AIComponent>();
    if(aiComponent && aiComponent->isInMode(AIMode::IDLE)) {
        speedModifier *= 0.5f;
    }

    speed *= speedModifier;

    auto transform = entity()->transform();
    lastTransform_ = transform;
    transform.position += velocity() * speed * dt;
    entity()->setTransform(transform);
}

void VelocityComponent::postUpdate(const f32 dt) {
}

f32 VelocityComponent::speed() const {
    return speed_;
}

void VelocityComponent::setAIControl(bool aiControl) {
    aiControled_ = aiControl;
}

void VelocityComponent::clearMotion() {
    motionVector_ = {0.0f, 0.0f};
}

void VelocityComponent::setMotion(MovementDirection direction) {
    movementDirection_ |= static_cast<u8>(direction);
    motionVector_ = movementDirectionToVector();
}

void VelocityComponent::setMotion(const Vec2& direction) {
    motionVector_ = direction;
}

void VelocityComponent::stopMotion(MovementDirection direction) {
    movementDirection_ &= ~static_cast<u8>(direction);
    motionVector_ = movementDirectionToVector();
}

void VelocityComponent::setSpeed(f32 speed) {
    speed_ = speed;
}

Vec2 VelocityComponent::movementDirectionToVector() const {
    Vec2 vec = {0.0, 0.0};
    if((movementDirection_ & static_cast<u8>(MovementDirection::NORTH)) != 0) {
        vec += {0, -1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::EAST)) != 0) {
        vec += {1, 0};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::SOUTH)) != 0) {
        vec += {0, 1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::WEST)) != 0) {
        vec += {-1, 0};
    }
    return vec;
}
