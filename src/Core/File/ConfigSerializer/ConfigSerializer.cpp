#include "ConfigSerializer.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "AppConfig.h"
#include "FileError.h"
#include "GraphicsConfig.h"
#include "Result.h"
#include "WindowConfig.h"

using namespace nlohmann;
namespace fs = std::filesystem;

namespace File {

static Result<Config::Window, Error> readWindowConfig(const json& data) {
    auto config = Config::Window{};

    data["height"].get_to(config.height);
    data["width"].get_to(config.width);

    auto state_str = data["state"].get<std::string>();
    if (state_str == "windowed")
        config.state = Config::WindowState::Windowed;
    else if (state_str == "borderless")
        config.state = Config::WindowState::Borderless;
    else if (state_str == "fullscreen")
        config.state = Config::WindowState::Fullscreen;
    else
        return Error::ParseError;

    return config;
}

static Result<Config::Graphics, Error> readGraphicsConfig(const json& data) {
    auto config = Config::Graphics{};

    data["height"].get_to(config.render_height);
    data["width"].get_to(config.render_width);

    auto buffering_str = data["buffering"].get<std::string>();
    if (buffering_str == "no_buffering")
        config.buffering_mode = Config::BufferingMode::NoBuffering;
    else if (buffering_str == "vsync_no_buffering")
        config.buffering_mode = Config::BufferingMode::VSyncNoBuffering;
    else if (buffering_str == "double_buffering")
        config.buffering_mode = Config::BufferingMode::DoubleBuffering;
    else if (buffering_str == "triple_buffering")
        config.buffering_mode = Config::BufferingMode::TripleBuffering;
    else
        return Error::ParseError;

    return config;
}

Result<Config::AppConfig, Error> readConfig(const fs::path& path) {
    if (fs::exists(path) == false) return Error::NotFound;

    auto file = std::ifstream(path);
    auto data = nlohmann::json();

    file >> data;

    try {
        auto name = data["name"].get<std::string>();

        auto graphics_config = readGraphicsConfig(data["graphics"]);
        auto window_config = readWindowConfig(data["window"]);

        if (graphics_config.isError()) return graphics_config.getError();
        if (window_config.isError()) return window_config.getError();

        return Config::AppConfig{name, graphics_config.getResult(),
                                 window_config.getResult()};
    } catch (const json::exception& ex) {
        return Error::ParseError;
    }
}

}  // namespace File