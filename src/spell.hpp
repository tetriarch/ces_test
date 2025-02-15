#pragma once

#include "entity.hpp"
#include "utils.hpp"

enum class ActionType {
    AOE,
    ARC,
    BEAM,
    PROJECTILE
};

enum class HitType {
    DAMAGE,
    DOT,
    HEAL
};

enum class DamageType {
    PHYSICAL,
    FIRE,
    COLD,
    LIGHTNING,
    POSION
};

struct DamageRange {
    u32 min;
    u32 max;
};

struct OnHitAction {
    HitType type;
    DamageType damageType;
    DamageRange damageRange;
    u32 periodicDamage;
    f32 duration;

};

struct Motion {};

struct ConstantMotion : Motion {
    f32 speed;
    void apply(const EntityPtr& target, f32 deltaTime) {
        Transform targetTransform = target->getTransform();
        Vec2 direction = directionFromAngle(targetTransform.rotationInDegrees);
        Vec2 velocity = direction * speed * deltaTime;
        targetTransform.position += velocity;
        target->setTransform(targetTransform);
    }
};

struct Action {
    ActionType type;
    bool pierce;
    Motion motion;
    std::vector<OnHitAction> actions;
};


struct Spell {
    std::string name;
    f32 castTime;
    f32 interruptTime;
    u32 manaCost;
    f32 cooldown;
    std::vector<Action> actions;
};