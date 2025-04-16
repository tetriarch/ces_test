#include "random_number_generator.hpp"

RandomNumberGenerator::RandomNumberGenerator() {
    std::random_device seed;
    generator_ = std::mt19937_64(seed());
}

s32 RandomNumberGenerator::getInt(s32 min, s32 max) {

    std::uniform_int_distribution<> distrib(min, max);
    return distrib(generator_);
}

f32 RandomNumberGenerator::getFloat(f32 min, f32 max) {

    std::uniform_real_distribution<> distrib(min, max);
    return distrib(generator_);
}

u32 RandomNumberGenerator::getUnsigned(u32 min, u32 max) {

    std::uniform_int_distribution<> distrib(min, max);
    return distrib(generator_);
}
