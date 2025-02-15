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

    Vec2 operator+=(Vec2 addition) {
        return {x + addition.x, y + addition.y};
    }
};

struct Rect {
    f32 x, y, w, h;
};

struct Transform {
    Vec2 position;
    f32 rotationInDegrees;
};

inline Vec2 directionFromAngle(f32 angleInDegrees) {
    f32 angleInRadians = angleInDegrees * (M_PI / 180.0f);
    Vec2 direction;
    direction.x = cos(angleInRadians);
    direction.y = sin(angleInRadians);
    return direction.normalized();
}

inline f32 lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }