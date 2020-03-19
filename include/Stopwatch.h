// src: https://stackoverflow.com/a/12883731

#ifndef MASTER_ARBEIT_STOPWATCH_H
#define MASTER_ARBEIT_STOPWATCH_H
#include <chrono>
#include <iostream>
namespace pathFinder {
template<typename Clock = std::chrono::steady_clock>
class Stopwatch {
    typename Clock::time_point last_;

public:
    Stopwatch()
            : last_(Clock::now()) {}

    void reset() {
        *this = Stopwatch();
    }

    typename Clock::duration elapsed() const {
        return Clock::now() - last_;
    }

    typename Clock::duration tick() {
        auto now = Clock::now();
        auto elapsed = now - last_;
        last_ = now;
        return elapsed;
    }
};

template<typename T, typename Rep, typename Period>
T duration_cast(const std::chrono::duration<Rep, Period> &duration) {
    return duration.count() * static_cast<T>(Period::num) / static_cast<T>(Period::den);
}
}
#endif //MASTER_ARBEIT_STOPWATCH_H
