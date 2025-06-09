#pragma once

#include "../component.hpp"
#include "../random_number_generator.hpp"
#include "spell.hpp"

struct SpellEffect;

struct StatusEffectData : public IAsset {
    std::string textureFilePath;
    bool animated;
    std::unordered_map<std::string, std::string> animationFiles;
    Rect rect;
};

class StatusEffectComponent : public Component<StatusEffectComponent> {
public:
    void applyEffect(const SpellEffect& effect);
    void applyDirectEffect(const SpellEffect& effect);

    /// @brief Removes activeEffect from effects_
    /// @param type of the effect e.g SpellEffecType::HASTE
    /// @return true if successfuly removed effect, false if failed
    bool removeEffect(SpellEffectType type);
    auto effect(SpellEffectType type) const -> const SpellEffect*;

    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

    bool isUnderEffect(SpellEffectType type) const;

private:
    void updateEffect(SpellEffect& effect, const f32 dt);

private:
    std::unordered_map<SpellEffectType, SpellEffect> effects_;
    std::vector<SpellEffectType> effectsToRemove_;
    RandomNumberGenerator rng_;
};
