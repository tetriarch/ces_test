#pragma once

#include "../utils.hpp"

struct DirectDamage {
    u32 min;
    u32 max;
};

struct DamageOverTime {
    u32 periodicDamage;
    f32 duration;
};

struct Slow {
    f32 magnitude;
    f32 duration;
};

struct Haste {
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

struct HealOverTime {
    u32 periodicHeal;
    f32 duration;
};