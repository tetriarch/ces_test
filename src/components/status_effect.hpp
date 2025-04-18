#pragma once

#include "../component.hpp"

#include "../global/effect_types.hpp"

using Buff = std::variant<Haste, HealOverTime>;
using Debuff = std::variant<DamageOverTime, Slow, Stun>;

template<typename T, typename V>
struct isInVariant;

template<typename T, typename... Types>
struct isInVariant<T, std::variant<Types...>> : std::disjunction<std::is_same<T, Types>...> {};

template<typename T, typename V>
inline constexpr bool isInVariantV = false;

template<typename T, typename... Types>
inline constexpr bool isInVariantV<T, std::variant<Types...>> = (std::is_same_v<T, Types> || ...);

template<typename T>
constexpr bool isBuff = isInVariantV<T, Buff>;

template<typename T>
constexpr bool isDebuff = isInVariantV<T, Debuff>;

template<typename T>
inline constexpr bool alwaysFalse = false;

struct ActiveBuff {
    Buff effect;
    f32 currentDuration{0.0f};
};

struct ActiveDebuff {
    Debuff effect;
    f32 currentDuration{0.0f};
};

class StatusEffectComponent : public Component<StatusEffectComponent> {

public:
    void addBuff(Buff effect);
    void addDebuff(Debuff effect);

    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;

    template<typename T>
    bool isUnderEffect();

private:
    std::vector<ActiveBuff> buffs_;
    std::vector<ActiveDebuff> debuffs_;
};

template<typename T>
inline bool StatusEffectComponent::isUnderEffect() {

    if constexpr(isBuff<T>) {
        for(auto& b : buffs_) {
            if(std::holds_alternative<T>(b.effect)) {
                return true;
            }
        }
    }
    else if constexpr(isDebuff<T>) {
        for(auto& d : debuffs_) {
            if(std::holds_alternative<T>(d.effect)) {
                return true;
            }
        }
    }
    else {
        static_assert(alwaysFalse<T>, "T has to be Buff or Debuff type");
    }

    return false;
}