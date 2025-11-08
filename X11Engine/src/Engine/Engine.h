#pragma once

#include <memory>

#include "AnimationRegistry.h"
#include "PhysicsFactory.h"
#include "Renderer.h"
#include "Window.h"
#include "World.h"

namespace Engine {

class Engine {
   public:
    static Engine& get() {
        static Engine instance;
        return instance;
    }

    bool init(std::shared_ptr<IResourceFactory> resource_factory);

    void run();
    void exit();

    World& getWorld();

   private:
    Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void update(float delta_time);

    void setupSystemPipeline();

    void setupPreUpdateStep();
    void setupUpdateStep();
    void setupPostUpdateStep();
    void setupPreSimulateStep();
    void setupSimulateStep();
    void setupPostSimulateStep();
    void setupRenderingStep();

    bool should_exit;

    std::shared_ptr<IResourceFactory> factory;
    std::shared_ptr<PhysicsFactory> physics;

    Renderer renderer;

    World world;
    std::shared_ptr<AnimationRegistry> animation_registry;
};

};  // namespace Engine