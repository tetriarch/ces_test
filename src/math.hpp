#pragma once

#include "utils.hpp"
#include <cmath>
#include <algorithm>



struct Vec2 {
    f32 x, y;

    inline Vec2 normalized() const {
        f32 length = this->length();
        if(length > 0) {
            return {x / length, y / length};
        }
        return {0.0f, 0.0f};
    }

    inline f32 length() const {
        f32 length = sqrtf(x * x + y * y);
        return length;
    }

    inline Vec2 operator*=(f32 scale) {
        x *= scale;
        y *= scale;
        return *this;
    }

    inline Vec2 operator*(f32 scale) const {
        return {x * scale, y * scale};
    }

    friend inline Vec2 operator*(f32 scale, const Vec2& v) {
        return {v.x * scale, v.y * scale};
    }

    inline Vec2& operator+=(const Vec2& addition) {
        x += addition.x;
        y += addition.y;
        return *this;
    }

    inline Vec2 operator+(const Vec2& addition) const {
        return{x + addition.x, y + addition.y};
    }

    inline Vec2& operator-=(const Vec2& substraction) {
        x -= substraction.x;
        y -= substraction.y;
        return *this;
    }

    inline Vec2 operator-(const Vec2& substraction) const {
        return{x - substraction.x, y - substraction.y};
    }
};

struct Line {
    Vec2 p1, p2;
    Line rotate(f32 angleInDegrees, const Vec2* pivotPoint = nullptr);
};

struct Circle {
    f32 x, y, r;
};

struct Rect {
    f32 x, y, w, h;
    Rect rotate(f32 angleInDegrees, const Vec2* pivotPoint = nullptr);

};

struct Box {
    Vec2 tl, tr, br, bl;
    Box rotate(f32 angleInDegrees, const Vec2* pivotPoint = nullptr);
};

struct Transform {
    Vec2 position;
    f32 rotationInDegrees;
};

namespace math {
    /// @brief Calculates linear interpolation between 2 values
    /// @param a min value
    /// @param b max value
    /// @param t 0.0 - 1.0 to interpolate min and max value with
    /// @return result of interpolation
    inline f32 lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

    inline f32 radians(f32 angleInDegrees) { return angleInDegrees * (static_cast<f32>(M_PI) / 180.0f); }

    inline f32 degrees(f32 angleInRadians) { return angleInRadians * (180.0f / static_cast<f32>(M_PI)); }
    inline f32 cross(const Vec2& l, const Vec2& r) { return l.x * r.y - l.y * r.x; }
    inline f32 dot(const Vec2& l, const Vec2& r) { return l.x * r.x + l.y * r.y; }
}

/// @brief calculates directional vector based on angle in degrees
/// @param angleInDegrees 
/// @return directional vector
inline Vec2 directionFromAngle(f32 angleInDegrees) {
    f32 radians = math::radians(angleInDegrees);
    Vec2 direction;
    direction.x = cos(radians);
    direction.y = sin(radians);
    return direction;
}

