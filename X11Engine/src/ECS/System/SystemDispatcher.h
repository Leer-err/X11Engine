#ifndef SYSTEM_DISPATCHER_H
#define SYSTEM_DISPATCHER_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ISystem.h"

class World;

class SystemDispatcher {
   public:
    SystemDispatcher(World& world) : world(world) {}

    template <typename System, typename... Args>
    void add(Args&&... args) {
        static_assert(std::is_base_of_v<ISystem, System>,
                      "System must inherit from ISystem interface");

        auto system = std::make_unique<System>(std::forward<Args>(args)...);
        systems.emplace_back(std::move(system));
    }

    void progress(World& world, float delta_time) {
        for (const auto& system : systems) system->preSimulate(world);
        for (const auto& system : systems) system->simulate(world);
        for (const auto& system : systems) system->preUpdate(world);
        for (const auto& system : systems) system->update(world, delta_time);
        for (const auto& system : systems) system->preRender(world);
        for (const auto& system : systems) system->render(world);
    }

   private:
    SystemDispatcher(const SystemDispatcher&) = delete;
    SystemDispatcher& operator=(const SystemDispatcher&) = delete;
    SystemDispatcher(SystemDispatcher&&) = delete;
    SystemDispatcher& operator=(SystemDispatcher&&) = delete;

    World& world;

    std::vector<std::unique_ptr<ISystem>> systems;
};

#endif  // SYSTEM_DISPATCHER_H