#include "Engine.h"

#include <chrono>
#include <memory>
#include <tracy/Tracy.hpp>

#include "GameInputConfigReader.h"
#include "PhysicalInput.h"
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

    return true;
}

void Engine::run() {
    // auto cam = StaticProjectionCamera(60, (float)4 / 3, 0.1f, 1000.f);

    // // auto model_reader = AssimpModelReader(factory);
    // // auto model =
    // //
    // model_reader.read("E:\\repos\\X11Engine\\X11Engine\\Assets\\Taunt.fbx");

    // // Entity player = world.createEntity();
    // // player.set<Name>({"Player"});
    // // player.add<Player>();
    // // Transform player_transform = {};
    // // player_transform.setPosition({0, 0, -5});
    // // player.set(player_transform);

    // // Entity camera = world.createEntity();
    // // camera.set<Name>({"Camera"});
    // // camera.set<Parent>({player});
    // // camera.set(cam);
    // // Transform camera_transform = {};
    // // camera_transform.setPosition({0, 5, 0});
    // // camera.set(camera_transform);

    // // player.set<Children>({{camera}});

    // // Shape plane = physics->createPlaneShape(false);
    // // auto plane_body = physics->createRigidStatic(Vector3(0, 0, 0),
    // //                                              Quaternion(0, 0, 1.57),
    // //                                              plane);

    // // Entity vampire = world.createEntity();
    // // vampire.set<Name>({"Vampire"});
    // // Shape shape = physics->createCapsuleShape(0.6, 0.25, false);
    // // {
    // //     auto body = physics->createRigidDynamic(shape, 1, &vampire);
    // //     body->setCCD(true);
    // //     body->lock(DynamicRigidBody::LockRotationX |
    // //                DynamicRigidBody::LockRotationZ);
    // //     vampire.set<DynamicRigidBodyComponent>({body});
    // // }
    // // vampire.set<AnimatedMesh>({model.value()});
    // // vampire.set<CurrentAnimation>({0, "mixamo.com"});

    // // Transform vampire_transform = {};
    // // vampire_transform.setPosition({0, 0, 0});
    // // vampire.set(vampire_transform);

    // // Entity projectile = world.createEntity();
    // // projectile.set<Name>({"Projectile"});
    // // Shape projectile_shape = physics->createSphereShape(0.5, true);
    // // auto projetile_body =
    // //     physics->createRigidDynamic(projectile_shape, 1, &projectile);
    // // projetile_body->setGravity(false);
    // // projectile.set<DynamicRigidBodyComponent>({projetile_body});
    // // projectile.set<Velocity>({Vector3(0, 0, 2)});
    // // projectile.add<DeleteProjectile>();

    // // Transform projectile_transform = {};
    // // projectile_transform.setPosition({0, 0.5, -4});
    // // projectile.set<Transform>(projectile_transform);

    GameInputConfigReader input_config_reader;
    input_config_reader.read(
        "E:\\repos\\X11Engine\\X11Engine\\src\\Data\\Input\\Config.json",
        GameInputContext::get());

    std::chrono::high_resolution_clock clock;
    auto start = clock.now();

    while (should_exit == false) {
        std::chrono::duration<float> elapsed = clock.now() - start;
        float delta_time = elapsed.count();
        start = clock.now();

        update(delta_time);
    }
}

void Engine::update(float delta_time) {
    ZoneScoped;
    PhysicalInput::get().saveState();

    renderer.beginFrame();

    world.progress(delta_time);

    renderer.endFrame();
    FrameMark;
}

void Engine::exit() { should_exit = true; }

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

World& Engine::getWorld() { return world; }
};  // namespace Engine