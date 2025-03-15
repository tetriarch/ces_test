#include "time.hpp"
#include <cmath>

constexpr f32 MAX_FRAME_TIME = 0.25f;

Time::Time() :
    currentTime_(std::chrono::high_resolution_clock::now()),
    accumulatedTime_(0.0f) {

}

Time& Time::get() {
    static Time instance;
    return instance;
}

void Time::update() {

    TimePoint newTime = std::chrono::high_resolution_clock::now();
    TimeDuration frameTime = newTime - currentTime_;

    if(frameTime.count() > MAX_FRAME_TIME) {
        frameTime = TimeDuration(MAX_FRAME_TIME);
    }

    currentTime_ = newTime;
    accumulatedTime_ += frameTime.count();
}

bool Time::isTimeToUpdate() {

    if(accumulatedTime_ >= DELTA_TIME) {
        accumulatedTime_ -= DELTA_TIME;

        return true;
    }
    return false;
}

f32 Time::alpha() {
    return accumulatedTime_ / DELTA_TIME;
}
