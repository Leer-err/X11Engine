#ifndef SYSTEM_DISPATCHER_H
#define SYSTEM_DISPATCHER_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "BasicPipeline/BasicPipelineSystems.h"
#include "ISystem.h"
#include "TypeId.h"
#include "TypeIdHelper.h"

class SystemDispatcher {
   public:
    class SystemDescription {
        friend class SystemDispatcher;

       protected:
        SystemDescription(std::function<void(std::vector<TypeId>&&)> callback)
            : callback(callback) {}

        SystemDescription& operator=(SystemDescription&&) = default;
        SystemDescription(SystemDescription&&) = default;

       public:
        ~SystemDescription() { callback(std::move(dependencies)); }

        template <typename SystemType>
        SystemDescription& dependsOn() {
            static_assert(std::is_base_of_v<ISystem, SystemType>,
                          "Dependency must inherit from ISystem interface");

            TypeId id = TypeIdHelper::getTypeId<SystemType>();

            dependencies.push_back(id);

            return *this;
        }

       private:
        SystemDescription& operator=(const SystemDescription&) = delete;
        SystemDescription(const SystemDescription&) = delete;

        std::vector<TypeId> dependencies;
        std::function<void(std::vector<TypeId>&&)> callback;
    };

    template <typename System, typename... Args>
    SystemDescription add(Args&&... args) {
        static_assert(std::is_base_of_v<ISystem, System>,
                      "System must inherit from ISystem interface");

        auto system = std::make_unique<System>(std::forward<Args>(args)...);

        system->prepare();

        TypeId id = TypeIdHelper::getTypeId<System>();

        systems.emplace(id, std::move(system));
        system_dependencies.emplace(id, std::vector<TypeId>{});

        auto callback = [=](std::vector<TypeId>&& dependencies) {
            system_dependencies[id] = dependencies;
            sortSystems();
        };
        return SystemDescription(callback);
    }

    void progress(World& world, float delta_time) {
        for (int i = 0; i < execution_order.size(); i++) {
            std::vector<ISystem*> generation = execution_order[i];

            for (int j = 0; j < generation.size(); j++) {
                generation[j]->update(world, delta_time);
            }
        }
    }

   private:
    void sortSystems() {
        constexpr int NO_GENERATION = -1;

        execution_order.clear();

        std::unordered_map<TypeId, int> generations;
        for (const auto& [id, system] : systems) {
            generations.emplace(id, NO_GENERATION);
        }

        while (std::any_of(generations.begin(), generations.end(),
                           [=](std::pair<TypeId, int> gen) {
                               return gen.second == NO_GENERATION;
                           })) {
            for (const auto& [id, dependencies] : system_dependencies) {
                if (generations[id] != NO_GENERATION) continue;

                if (dependencies.size() == 0) {
                    generations[id] = 0;
                }

                if (std::any_of(
                        dependencies.begin(), dependencies.end(),
                        [=, &generations](TypeId dependency) {
                            if (systems.find(dependency) == systems.end())
                                return true;
                            return generations[dependency] == NO_GENERATION;
                        })) {
                    continue;
                }

                size_t generation = 0;
                for (const auto& dependency : dependencies) {
                    if (generations[dependency] >= generation) {
                        generation = generations[dependency] + 1;
                    }
                }
                generations[id] = generation;
            }
        }

        for (int current_generation = 0;; current_generation++) {
            std::vector<ISystem*> generation;

            for (const auto& [id, generation_index] : generations) {
                if (generation_index == current_generation) {
                    generation.push_back(systems[id].get());
                }
            }

            if (generation.size() == 0) {
                break;
            }

            execution_order.push_back(generation);
        }
    }

    std::unordered_map<TypeId, std::unique_ptr<ISystem>> systems;
    std::unordered_map<TypeId, std::vector<TypeId>> system_dependencies;
    std::vector<std::vector<ISystem*>> execution_order;
};

#endif  // SYSTEM_DISPATCHER_H