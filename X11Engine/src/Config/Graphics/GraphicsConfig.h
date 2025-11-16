#ifndef GRAPHICS_CONFIG_H
#define GRAPHICS_CONFIG_H

#include <cstdint>

enum class WindowState { Windowed, Borderless, Fullscreen };

struct GraphicsConfig {
    uint32_t render_width;
    uint32_t render_height;

    WindowState window_state;
};

#endif  // GRAPHICS_CONFIG_H