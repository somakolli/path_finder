// src: https://stackoverflow.com/a/12883731

#pragma once
#include <chrono>
#include <iostream>
namespace pathFinder {
template <typename Clock = std::chrono::high_resolution_clock>
class Stopwatch {
  typename Clock::time_point _last;

public:
  Stopwatch() : _last(Clock::now()) {}

  void reset() { *this = Stopwatch(); }

  typename Clock::duration elapsed() const { return Clock::now() - _last; }
  [[maybe_unused]] double elapsedMil() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - _last).count();
  }
  [[nodiscard]] double elapsedMicro() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - _last).count();
  }
};
} // namespace pathFinder
