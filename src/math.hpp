#pragma once

#include "utils.hpp"
#include <cmath>

struct Vec2 {
    f32 x, y;

    Vec2 normalized() const {
        f32 length = sqrtf(x * x + y * y);
        if(length > 0) {
            return {x / length, y / length};
        }
        return {0.0f, 0.0f};
    }

    Vec2 operator*(f32 scale) const {
        return {x * scale, y * scale};
    }
};

struct Rect {
    f32 x, y, w, h;
};

struct Transform {
    Vec2 position;
    // can't be bothered with rotation and scale
};

f32 lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }