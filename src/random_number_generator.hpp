#pragma once

#include "utils.hpp"

#include <random>

class RandomNumberGenerator {

public:
    RandomNumberGenerator();
    ~RandomNumberGenerator() = default;

public:
    s32 getInt(s32 min, s32 max);
    f32 getFloat(f32 min, f32 max);
    u32 getUnsigned(u32 min, u32 max);
private:
    std::mt19937_64 generator_;

private:
};

#define RNG RandomNumberGenerator::get()