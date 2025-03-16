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

    Vec2& operator+=(const Vec2& addition) {
        x += addition.x;
        y += addition.y;
        return *this;
    }

    Vec2 operator+(Vec2& addition) const {
        return{x + addition.x, y + addition.y};
    }

    Vec2& operator-=(const Vec2& substraction) {
        x -= substraction.x;
        y -= substraction.y;
        return *this;
    }

    Vec2 operator-(Vec2& substraction) const {
        return{x - substraction.x, y - substraction.y};
    }
};

struct Rect {
    f32 x, y, w, h;
};

struct Transform {
    Vec2 position;
    f32 rotationInDegrees;
};

/// @brief calculates directional vector based on angle in degrees
/// @param angleInDegrees 
/// @return directional vector
inline Vec2 directionFromAngle(f32 angleInDegrees) {
    f32 angleInRadians = angleInDegrees * (static_cast<f32>(M_PI) / 180.0f);
    Vec2 direction;
    direction.x = cos(angleInRadians);
    direction.y = sin(angleInRadians);
    return direction;
}

/// @brief Calculates linear interpolation between 2 values
/// @param a min value
/// @param b max value
/// @param t 0.0 - 1.0 to interpolate min and max value with
/// @return result of interpolation
namespace math {
    inline f32 lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }
}