#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "IRenderEngine.h"
#include "IScene.h"

// #include "Scene.h"
// #include "World.h"

namespace Engine {

class Engine {
   public:
    static Engine& get() {
        static Engine instance;
        return instance;
    }

    bool init();

    void run(IScene* scene);
    void exit();

    float getTime() const;

   private:
    Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void windowWorker();
    void mainLoopWorker(IScene* scene);

    void update(IScene* scene, float delta_time);

    void setupSystemPipeline();

    void setupPreUpdateStep();
    void setupUpdateStep();
    void setupPostUpdateStep();
    void setupPreSimulateStep();
    void setupSimulateStep();
    void setupPostSimulateStep();
    void setupRenderingStep();

    std::atomic_bool should_exit;

    std::thread main_loop_thread;

    std::chrono::high_resolution_clock clock;
    std::chrono::high_resolution_clock::time_point last_elapsed;
    std::chrono::high_resolution_clock::time_point start;

    float fps;

    Graphics::IRenderEngine* renderer;
    // std::shared_ptr<PhysicsFactory> physics;
    // std::shared_ptr<AnimationRegistry> animation_registry;
};

};  // namespace Engine