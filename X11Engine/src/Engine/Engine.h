#pragma once

#include <chrono>
#include <memory>

#include "Renderer.h"
#include "Scene.h"
#include "Window.h"
#include "World.h"

namespace Engine {

class Engine {
   public:
    static Engine& get() {
        static Engine instance;
        return instance;
    }

    bool init();

    void run();
    void exit();

    float getTime() const;

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

    std::chrono::high_resolution_clock clock;
    std::chrono::high_resolution_clock::time_point last_elapsed;
    std::chrono::high_resolution_clock::time_point start;

    // std::shared_ptr<PhysicsFactory> physics;
    // std::shared_ptr<AnimationRegistry> animation_registry;
};

};  // namespace Engine