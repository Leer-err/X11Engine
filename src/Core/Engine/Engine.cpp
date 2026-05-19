#include "Engine.h"

#include <stb_image.h>

#include <chrono>
#include <thread>
#include <tracy/Tracy.hpp>

#include "Graphics.h"
#include "GraphicsCommunicationManager.h"
#include "IScene.h"
#include "Overlay.h"
#include "PhysicalInput.h"
#include "Window.h"

namespace Engine {

Engine::Engine() : should_exit(false) {}

bool Engine::init() {
    renderer = Graphics::getRenderEngine();

    Graphics::Overlay::get().add<Graphics::OverlayElements::Text>(
        "General", [this]() { return std::format("FPS is {}", fps); });

    return true;
}

void Engine::run(IScene* scene) {
    main_loop_thread = std::thread([this, scene]() { mainLoopWorker(scene); });

    windowWorker();

    main_loop_thread.join();
}

void Engine::windowWorker() {
    while (Window::get().processMessages()) {
    }

    should_exit = true;
}

void Engine::mainLoopWorker(IScene* scene) {
    last_elapsed = clock.now();
    start = clock.now();

    while (should_exit == false) {
        auto elapsed = clock.now();
        float delta_time =
            std::chrono::duration_cast<std::chrono::duration<float>>(
                elapsed - last_elapsed)
                .count();
        last_elapsed = elapsed;

        update(scene, delta_time);
    }
}

void Engine::update(IScene* scene, float delta_time) {
    ZoneScoped;

    fps = 1.f / delta_time;

    Input::PhysicalInput::get().saveState();

    scene->update(delta_time);

    renderer->render();

    GraphicsCommunicationManager::get().clear();

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