#pragma once

#include "../entity.hpp"
#include "../utils.hpp"

enum class ActionType {
    AOE,
    ARC,
    BEAM,
    PROJECTILE,
    UNKNOWN
};

enum class HitEffect {
    DAMAGE,
    DOT,
    HEAL,
    SLOW,
    STUN,
    UNKNOWN
};

enum class DamageType {
    PHYSICAL,
    FIRE,
    COLD,
    LIGHTNING,
    POSION,
    UNKNOWN
};

struct DamageRange {
    u32 min;
    u32 max;
};

struct DamageOverTime {
    u32 periodicDamage;
    f32 duration;
};

union DamageDelivery {
    DamageRange damageRange;
    DamageOverTime damageOverTime;
};

struct OnHitAction {
    HitEffect type;
    DamageType damageType;
    DamageDelivery damage;
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

struct InstantMotion : Motion {
    void apply(const EntityPtr& target) {
        //TODO: Unsure of the logic here yet
        //for snap action abilities, beams like Zap
    }
};

struct SpellAction {
    ActionType type;
    bool pierce;
    std::shared_ptr<Motion> motion;
    std::vector<OnHitAction> actions;
};


class Spell : public Component<Spell> {
public:
    Spell() = default;
    Spell(std::string name, f32 castTime, f32 interruptTime, s32 manaCost, f32 cooldown);
    void addAction(const SpellAction& action);
    auto describe() -> std::string;

private:
    std::string name_;
    f32 castTime_;
    f32 interruptTime_;
    s32 manaCost_;
    f32 cooldown_;
    std::vector<SpellAction> actions_;
};