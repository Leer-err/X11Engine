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
    Engine();

    bool init(std::shared_ptr<IResourceFactory> resource_factory,
              std::shared_ptr<Window> window);

    void run();

    void exit();

   private:
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

    std::shared_ptr<Window> window;
};

};  // namespace Engine