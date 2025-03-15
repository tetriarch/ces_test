#pragma once

#include "utils.hpp"

// based on article https://gafferongames.com/post/fix_your_timestep/ by Glenn Fiedler

class Time {
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using TimeDuration = std::chrono::duration<f32>;

public:
    static Time& get();
    const f32 DELTA_TIME = 0.01f;

public:
    void update();
    bool isTimeToUpdate();
    f32 alpha();

private:
    TimePoint currentTime_;
    f32 accumulatedTime_;

private:
    Time();
    ~Time() = default;
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&&) = delete;
};