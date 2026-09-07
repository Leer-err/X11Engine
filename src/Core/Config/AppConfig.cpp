#include "AppConfig.h"

#include "GraphicsConfig.h"
#include "WindowConfig.h"

namespace Config {

App::App() { loadDefaults(); }

void App::loadDefaults() {
    name = "Name";

    graphics.render_width = 1280;
    graphics.render_height = 720;

    graphics.buffering_mode = BufferingMode::NoBuffering;

    window.width = 1280;
    window.height = 720;

    window.state = Config::WindowState::Windowed;
}

Graphics App::getGraphicsConfig() const { return graphics; }

Window App::getWindowConfig() const { return window; }

std::string App::getName() const { return name; }

};  // namespace Config