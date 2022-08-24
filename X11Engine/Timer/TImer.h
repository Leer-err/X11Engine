#pragma once

#include <chrono>

using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;

class Timer {
    static Timer* get() {
        static Timer instance;
        return &instance;
    }

    void Update() { m_lastUpdate = m_clock.now(); }

    float GetTime() { return (m_clock.now() - m_lastUpdate).count(); }

   private:
    Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    high_resolution_clock m_clock;
    time_point<high_resolution_clock> m_lastUpdate;
};