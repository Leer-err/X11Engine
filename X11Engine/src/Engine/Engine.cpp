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
    // physics = std::make_shared<PhysicsFactory>();
    // physics->init();

    // animation_registry = std::make_shared<AnimationRegistry>();
    // auto animation_reader = AssimpAnimationReader();
    // auto animations = animation_reader.readAll(
    //     "E:\\repos\\X11Engine\\X11Engine\\Assets\\Taunt.fbx");
    // for (auto animation : animations) {
    //     animation_registry->add(animation);
    // }

    setupSystemPipeline();

    GraphicsConfig config;
    config.render_height = 600;
    config.render_width = 800;
    config.window_state = WindowState::Windowed;
    Renderer::get().initializeResources(config);

    Scene::get();
    ::Engine::Script::ScriptLoader().loadFromDirectory(
        "E:\\repos\\X11Engine\\X11Engine\\Scripts");
    Script::ScriptSandbox::get().runFunction("Init");

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

void Engine::setupSystemPipeline() {
    setupPreUpdateStep();
    setupUpdateStep();
    setupPostUpdateStep();
    setupPreSimulateStep();
    setupSimulateStep();
    setupPostSimulateStep();
    setupRenderingStep();
}

void Engine::setupPreUpdateStep() {
    // world.addSystem<PreUpdate>();
}

void Engine::setupUpdateStep() {
    // world.addSystem<Update>().dependsOn<PreUpdate>();

    // world.addSystem<ScriptSystem>().dependsOn<Update>();
    // world.addSystem<DeleteProjectileSystem>().dependsOn<Update>();
}

void Engine::setupPostUpdateStep() {
    // world.addSystem<PostUpdate>().dependsOn<Update>();
}

void Engine::setupPreSimulateStep() {
    // world.addSystem<PreSimulation>().dependsOn<PostUpdate>();

    // world.addSystem<PreSimulateUpdateSystem>().dependsOn<PreSimulation>();
}

void Engine::setupSimulateStep() {
    // world.addSystem<Simulation>().dependsOn<PreSimulation>();

    // world.addSystem<PhysicsSystem>(physics, world, 1.f / 60)
    //     .dependsOn<Simulation>();
}

void Engine::setupPostSimulateStep() {
    // world.addSystem<PostSimulation>().dependsOn<Simulation>();

    // world.addSystem<PostSimulateUpdateSystem>(world)
    //     .dependsOn<PostSimulation>();
}

void Engine::setupRenderingStep() {
    // world.addSystem<Rendering>().dependsOn<PostSimulation>();

    // std::shared_ptr<IShaderReader> shader_reader =
    //     std::make_shared<D3DShaderReader>();

    // world.addSystem<StaticMeshRenderSystem>(factory, shader_reader)
    //     .dependsOn<Rendering>();
    // world
    //     .addSystem<AnimatedMeshRenderSystem>(factory, shader_reader,
    //                                          animation_registry)
    //     .dependsOn<Rendering>();
    // world.addSystem<OverlayRenderSystem>(factory).dependsOn<Rendering>();
}

// World& Engine::getWorld() { return world; }
};  // namespace Engine