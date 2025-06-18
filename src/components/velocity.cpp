#include "velocity.hpp"

#include "../entity.hpp"
#include "collision.hpp"
#include "owner.hpp"
#include "spell_book.hpp"
#include "status_effect.hpp"
#include "tag.hpp"

const f32 ON_CAST_MOVEMENT_SPEED_MULTIPLIER = 0.5f;

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
        speedModifier -= ON_CAST_MOVEMENT_SPEED_MULTIPLIER;
    }

    // modify speed when hasted or slowed
    auto statusEffect = entity()->component<StatusEffectComponent>();
    if(statusEffect) {
        auto haste = statusEffect->effect(SpellEffectType::HASTE);
        auto slow = statusEffect->effect(SpellEffectType::SLOW);
        auto stun = statusEffect->effect(SpellEffectType::STUN);

        if(haste) {
            speedModifier *= 1.0f + haste->magnitude;
        }
        if(slow) {
            speedModifier *= 1.0f - slow->magnitude;
        }
        if(stun) {
            speedModifier = 0.0f;
        }
    }

    speed *= speedModifier;

    auto transform = entity()->transform();
    lastTransform_ = transform;
    transform.position += velocity() * speed * dt;
    entity()->setTransform(transform);
}

void VelocityComponent::postUpdate(const f32 dt) {
    auto collisionComponent = entity()->component<CollisionComponent>();
    if(collisionComponent && collisionComponent->collided()) {
        auto tag = entity()->component<TagComponent>();
        auto colliders = collisionComponent->colliders();

        for(auto&& collider : colliders) {
            // collider is spell -- skip
            auto colliderSpellComponent = collider->component<SpellComponent>();
            if(colliderSpellComponent) {
                continue;
            }

            auto colliderOwnerComponent = collider->component<OwnerComponent>();
            if(!colliderOwnerComponent ||
                (colliderOwnerComponent && !colliderOwnerComponent->isOwnedBy(entity()))) {
                // push around only when we are not stunned
                auto statusEffectComponent = entity()->component<StatusEffectComponent>();
                if(statusEffectComponent &&
                    !statusEffectComponent->isUnderEffect(SpellEffectType::STUN)) {
                    auto transform = entity()->transform();
                    transform.position -= collisionComponent->collisionNormal() *
                                          collisionComponent->collisionDepth();
                    entity()->setTransform(transform);
                }
            }
        }
    }
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
