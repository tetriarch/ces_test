#pragma once

#include "../component.hpp"
#include "../random_number_generator.hpp"
#include "spell.hpp"

struct ActiveEffect {
    SpellEffect effect;
    f32 currentDuration{0.0f};
    u32 stacks = 0;
};

class SpellEffect;

class StatusEffectComponent : public Component<StatusEffectComponent> {

public:
    void addEffect(const SpellEffect& effect);

    /// @brief Removes activeEffect from effects_
    /// @param pointer to effect 
    /// @return true if successfuly removed effect, false if failed 
    bool removeEffect(ActiveEffect* effect);

    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

    bool isUnderEffect(SpellEffectType type) const;

private:
    void updateEffect(ActiveEffect& effect, const f32 dt);

private:
    std::vector<ActiveEffect> effects_;
    std::vector<ActiveEffect*> effectsToRemove_;
    RandomNumberGenerator rng_;
};