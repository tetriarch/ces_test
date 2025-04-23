#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../i_asset.hpp"
#include "../random_number_generator.hpp"
#include "../utils.hpp"

#include "../global/effect_types.hpp"

#include "collision.hpp"
#include "geometry.hpp"
#include "tag.hpp"

enum class ActionType {
    AOE,
    ARC,
    BEAM,
    PROJECTILE,
    SELF,
    UNKNOWN
};

// nature/element of the spell
enum class DamageType {
    PHYSICAL,
    FIRE,
    COLD,
    LIGHTNING,
    VOID,
    UNKNOWN
};

using SpellEffect = std::variant<
    DirectDamage,
    DamageOverTime,
    Slow,
    Haste,
    Stun,
    Heal,
    HealOverTime
>;

struct SpellEffectOnHit {
    DamageType damageType;
    FactionType targetFaction;
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

struct InstantMotion : Motion {};

struct SpellAction {
    ActionType type;
    bool pierce;
    std::shared_ptr<Motion> motion;
    std::vector<SpellEffectOnHit> effects;
};

struct SpellData : public IAsset {
    std::string name;
    f32 castTime;
    f32 interruptTime;
    u32 manaCost;
    f32 cooldown;
    f32 duration;
    f32 maxRange;
    std::string textureFilePath;
    std::vector<SpellAction> actions;
    GeometryData geometryData;
    CollisionData collisionData;
};

class SpellComponent : public Component<SpellComponent> {
public:
    SpellComponent(std::shared_ptr<SpellData> spellData);
    void attach() override;
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

private:
    bool canApplyEffect(EntityPtr applicant, EntityPtr target, SpellEffectOnHit onHitEffect);
    void applyEffect(EntityPtr applicant, EntityPtr target, SpellEffect effect, RandomNumberGenerator& rng);
    void applyEffect(EntityPtr target, DirectDamage damage);
    void applyEffect(EntityPtr target, DamageOverTime dot);
    void applyEffect(EntityPtr target, Slow dot);
    void applyEffect(EntityPtr target, Haste haste);
    void applyEffect(EntityPtr target, Stun stun);
    void applyEffect(EntityPtr target, Heal heal);
    void applyEffect(EntityPtr target, HealOverTime hot);


private:
    std::shared_ptr<SpellData> spellData_;
    f32 currentDuration_;
    f32 traveledDistance_;
    RandomNumberGenerator rng_;
    bool dead_;
};