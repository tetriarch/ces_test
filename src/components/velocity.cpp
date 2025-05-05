#include "../entity.hpp"

#include "collision.hpp"
#include "owner.hpp"
#include "spell_book.hpp"
#include "status_effect.hpp"
#include "tag.hpp"
#include "velocity.hpp"

const f32 ON_CAST_MOVEMENT_SPEED_MULTIPLIER = 0.5f;

Vec2 VelocityComponent::velocity() const {

    Vec2 velocityVector = {0,0};
    if((movementDirection_ & static_cast<u8>(MovementDirection::NORTH)) != 0) {
        velocityVector += {0, -1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::EAST)) != 0) {
        velocityVector += {1, 0};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::SOUTH)) != 0) {
        velocityVector += {0, 1};
    }
    if((movementDirection_ & static_cast<u8>(MovementDirection::WEST)) != 0) {
        velocityVector += {-1, 0};
    }
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

        if(haste) {
            speedModifier *= 1.0f + haste->magnitude;

        }
        if(slow) {
            speedModifier *= 1.0f - slow->magnitude;
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
            if(tag && tag->isTaggedAs(TagType::PLAYER)) {
                auto colliderOwnerComponent = collider->component<OwnerComponent>();
                if(!colliderOwnerComponent || (colliderOwnerComponent && !colliderOwnerComponent->isOwnedBy(entity()))) {
                    auto transform = entity()->transform();
                    transform.position -= collisionComponent->collisionNormal() * collisionComponent->collisionDepth();
                    entity()->setTransform(transform);
                }
            }
        }
    }
}

f32 VelocityComponent::speed() const {

    return speed_;
}

void VelocityComponent::setMotion(MovementDirection direction) {

    movementDirection_ |= static_cast<u8>(direction);
}

void VelocityComponent::stopMotion(MovementDirection direction) {

    movementDirection_ &= ~static_cast<u8>(direction);
}

void VelocityComponent::setSpeed(f32 speed) {
    speed_ = speed;
}
