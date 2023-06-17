#pragma once

#include <chrono>

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;

class Timer {
   public:
    static Timer* get() {
        static Timer instance;
        return &instance;
    }

    void Update() {
        time_point<high_resolution_clock> now = m_clock.now();
        m_delta = now - m_lastUpdate;
        m_lastUpdate = now;
    }

    inline double GetDeltaTime() const { return m_delta.count(); }

   private:
    Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    high_resolution_clock m_clock;
    time_point<high_resolution_clock> m_lastUpdate;
    duration<float, std::chrono::seconds::period> m_delta;
};