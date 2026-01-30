#ifndef WINDOW_CONFIG_H
#define WINDOW_CONFIG_H

#include <cstdint>

namespace Config {

struct Window {
    enum class State { Windowed, Borderless, Fullscreen };

    uint32_t width;
    uint32_t height;

    State state;
};

};  // namespace Config

#endif  // WINDOW_CONFIG_H