#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../i_asset.hpp"
#include "../utils.hpp"
#include "collision.hpp"
#include "geometry.hpp"
#include "tag.hpp"

enum class ActionType { AOE, ARC, BEAM, PROJECTILE, SELF, UNKNOWN };

// nature/element of the spell
enum class DamageType { PHYSICAL, FIRE, COLD, LIGHTNING, VOID, UNKNOWN };

enum class SpellEffectType {
    DIRECT_DAMAGE,
    DAMAGE_OVER_TIME,
    SLOW,
    HASTE,
    STUN,
    DIRECT_HEAL,
    HEAL_OVER_TIME,
    UNKNOWN
};

struct SpellEffect {
    std::string name{""};
    SpellEffectType type{SpellEffectType::UNKNOWN};
    f32 currentDuration{0.0f};
    f32 maxDuration{0.0f};
    DamageType dmgType{DamageType::UNKNOWN};
    FactionType targetFaction{FactionType::UNKNOWN};
    u32 minValue{0};
    u32 maxValue{0};
    u32 periodicValue{0};
    f32 magnitude{0.0f};
    u32 currentStacks{0};
    u32 maxStacks{1};
    bool visual;
    std::string effectFilePath{""};

    bool isDirect() const;
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
    std::vector<SpellEffect> effects;
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
    SpellAction action;
    GeometryData geometryData;
    CollisionData collisionData;
    bool animated{false};
    std::unordered_map<std::string, std::string> animationFiles;
    bool particles{false};
    std::vector<std::string> emitterFiles;
};

class SpellComponent : public Component<SpellComponent> {
public:
    SpellComponent(std::shared_ptr<SpellData> spellData);
    void attach() override;
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;
    bool isDead();

private:
    bool canApplyEffect(EntityPtr applier, EntityPtr target, SpellEffect onHitEffect);

private:
    std::shared_ptr<SpellData> spellData_;
    f32 currentDuration_;
    f32 traveledDistance_;
    bool dead_;
};
