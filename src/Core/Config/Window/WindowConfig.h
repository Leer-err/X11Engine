#ifndef WINDOW_CONFIG_H
#define WINDOW_CONFIG_H

#include <cstdint>

namespace Config {

enum class WindowState { Windowed, Borderless, Fullscreen };

struct Window {
    uint32_t width;
    uint32_t height;

    WindowState state;
};

};  // namespace Config

#endif  // WINDOW_CONFIG_H