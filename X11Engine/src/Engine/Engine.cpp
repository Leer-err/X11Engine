#include "Engine.h"

#include <chrono>
#include <tracy/Tracy.hpp>

#include "GameInputConfigReader.h"
#include "GraphicsConfig.h"
#include "Overlay.h"
#include "PhysicalInput.h"
#include "ScriptLoader.h"
#include "ScriptSandbox.h"
#include "World.h"

namespace Engine {

Engine::Engine() : should_exit(false) {}

bool Engine::init() {
    GraphicsConfig config;
    config.render_height = 600;
    config.render_width = 800;
    config.window_state = WindowState::Windowed;
    Renderer::get().initializeResources(config);

    Scene::get();
    ::Engine::Script::ScriptLoader().loadFromDirectory(
        "E:\\repos\\X11Engine\\X11Engine\\Scripts");
    Script::ScriptSandbox::get().runFunction("Init");

    Overlay::Overlay::get().add<Overlay::OverlayElements::Text>(
        "General", [this]() { return std::format("FPS is {}", fps); });

    return true;
}

void Engine::run() {
    GameInputConfigReader input_config_reader;
    input_config_reader.read(
        "E:\\repos\\X11Engine\\X11Engine\\src\\Data\\Input\\Config.json",
        GameInputContext::get());

    start = clock.now();
    while (should_exit == false) {
        auto elapsed = clock.now();
        float delta_time =
            std::chrono::duration_cast<std::chrono::duration<float>>(
                elapsed - last_elapsed)
                .count();
        last_elapsed = elapsed;

        update(delta_time);
    }
}

void Engine::update(float delta_time) {
    ZoneScoped;

    fps = 1.f / delta_time;

    PhysicalInput::get().saveState();

    Renderer::get().beginFrame();

    Scene::get().update(delta_time);

    Overlay::Overlay::get().draw();

    Renderer::get().endFrame();
    FrameMark;
}

void Engine::exit() { should_exit = true; }

float Engine::getTime() const {
    auto elapsed = clock.now();
    return std::chrono::duration_cast<std::chrono::duration<float>>(elapsed -
                                                                    start)
        .count();
}

};  // namespace Engine