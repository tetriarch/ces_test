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

// nature/element of the spell
enum class DamageType {
    PHYSICAL,
    FIRE,
    COLD,
    LIGHTNING,
    POSION,
    UNKNOWN
};

struct DirectDamage {
    s32 min;
    s32 max;
};

struct DamageOverTime {
    s32 periodicDamage;
    f32 duration;
};

struct Slow {
    s32 magnitude;
    f32 duration;
};

struct Stun {
    f32 duration;
};

struct Heal {
    s32 min;
    s32 max;
};

using SpellEffect = std::variant<DirectDamage, DamageOverTime, Slow, Stun, Heal>;

struct SpellEffectOnHit {
    DamageType damageType;
    SpellEffect effect;
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
    std::vector<SpellEffectOnHit> actions;
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