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

struct Motion {
    virtual ~Motion() = default;
};

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


struct SpellData {
    std::string name;
    f32 castTime;
    f32 interruptTime;
    s32 manaCost;
    f32 cooldown;
    std::vector<SpellAction> actions;
};



class SpellComponent : public Component<SpellComponent> {
public:
    SpellComponent(std::shared_ptr<SpellData> spellData);
    auto describe() -> std::string override;

private:
    std::shared_ptr<SpellData> spellData_;
};