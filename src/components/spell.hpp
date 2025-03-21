#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../utils.hpp"
#include "../i_asset.hpp"

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
    u32 min;
    u32 max;
};

struct DamageOverTime {
    u32 periodicDamage;
    f32 duration;
};

struct Slow {
    u32 magnitude;
    f32 duration;
};

struct Stun {
    f32 duration;
};

struct Heal {
    u32 min;
    u32 max;
};

using SpellEffect = std::variant<DirectDamage, DamageOverTime, Slow, Stun, Heal>;

struct SpellEffectOnHit {
    DamageType damageType;
    SpellEffect effect;
};

struct Motion {
    virtual ~Motion() = default;
    virtual void apply(const EntityPtr& target, f32 dt) {};
};

struct ConstantMotion : Motion {

    f32 speed{0};
    void apply(const EntityPtr& target, f32 dt) {

        Transform targetTransform = target->transform();
        Vec2 direction = directionFromAngle(targetTransform.rotationInDegrees);
        Vec2 velocity = direction * speed;
        targetTransform.position += velocity * dt;
        target->setTransform(targetTransform);
    }
};

struct InstantMotion : Motion { /* leave empty*/ };

struct SpellAction {
    ActionType type;
    bool pierce;
    std::shared_ptr<Motion> motion;
    std::vector<SpellEffectOnHit> effects;
};

enum class GeometryType {
    STATIC,
    DYNAMIC,
    UNKNOWN
};

struct SpellGeometry {
    Rect rect;
    GeometryType type;
};

struct SpellData : public IAsset {
    std::string name;
    f32 castTime;
    f32 interruptTime;
    u32 manaCost;
    f32 cooldown;
    f32 maxDistance;
    std::string textureFilePath;
    std::vector<SpellAction> actions;
    SpellGeometry geometry;
};

class SpellComponent : public Component<SpellComponent> {
public:
    SpellComponent(std::shared_ptr<SpellData> spellData);
    void update(f32 dt) override;

private:
    std::shared_ptr<SpellData> spellData_;
};