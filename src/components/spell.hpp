#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../i_asset.hpp"
#include "../utils.hpp"
#include "collision.hpp"
#include "geometry.hpp"
#include "tag.hpp"

enum class SpellRequirement : u8 {
    NONE = 0,
    GEOMETRY = 1 << 0,
    COLLISION = 1 << 1,
    ANIMATION = 1 << 2,
    PARTICLE = 1 << 3,
    SPAWN = 1 << 4
};

enum class ActionType { AOE, ARC, BEAM, PROJECTILE, SELF, SPAWN, UNKNOWN };

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
    EntityHandle applier;

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
    std::unordered_map<std::string, std::string> animationFiles;
    std::vector<std::string> emitterFiles;
    u8 componentRequirements{0};
    std::string spawnName{""};
    std::string spawnPrefabFile{""};

    bool requiresComponent(SpellRequirement req) const;
};

class SpellComponent : public Component<SpellComponent> {
public:
    SpellComponent();
    explicit SpellComponent(std::shared_ptr<SpellData> spellData);

    void attach() override;
    void update(f32 dt) override;
    void postUpdate(f32 dt) override;
    void setCasterTag(TagType tag);

private:
    enum class State {
        Alive,
        Dying,
        Dead
    };

    bool canApplyEffect(EntityPtr target, SpellEffect onHitEffect) const;

    std::shared_ptr<SpellData> spellData_;
    f32 currentDuration_ {0};
    f32 traveledDistance_ {0};
    State state_ { State::Alive };
    TagType casterTag_;
    size_t colliderListenerId_;
};
