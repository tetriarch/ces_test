#pragma once

#include "../component.hpp"

#include "../global/effect_types.hpp"

using StatusEffect = std::variant<
    DamageOverTime,
    Haste,
    HealOverTime,
    Slow,
    Stun
>;

enum class EffectType {
    BUFF,
    DEBUFF
};

// empty struct template for type traits (std::variant)
template<typename T, typename V>
struct isInVariant;

// specialization of isInVariant to check if T exists in std::variant<Types...>
// ... variadic table (multiple parameters basically)
// std::disjunction checks if T is same as any of the types in std::variant<Types...>
template<typename T, typename... Types>
struct isInVariant<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};

// Helper to check if T is status effect
template<typename T>
constexpr bool isStatusEffect = isInVariant<T, StatusEffect>();

struct ActiveEffect {
    StatusEffect effect;
    EffectType type;
    f32 currentDuration{0.0f};
};

class StatusEffectComponent : public Component<StatusEffectComponent> {

public:
    void addEffect(StatusEffect effect, EffectType type);

    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

    template<typename T>
    bool isUnderEffect() const;

private:
    std::vector<ActiveEffect> effects_;
};

template<typename T>
inline bool StatusEffectComponent::isUnderEffect() const {

    static_assert(isStatusEffect<T>, "T is not valid status effect");

    for(auto& e : effects_) {
        if(std::holds_alternative<T>(e.effect)) {
            return true;
        }
    }

    return false;
}