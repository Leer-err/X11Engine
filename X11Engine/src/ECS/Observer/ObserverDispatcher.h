#ifndef OBSERVER_DISPATCHER_H
#define OBSERVER_DISPATCHER_H

#include <functional>
#include <unordered_map>
#include <vector>

#include "EntityId.h"
#include "EntityRegistry.h"
#include "TypeId.h"

class Entity;

enum class Event { Add, Set };

class ObserverDispatcher {
   public:
    ObserverDispatcher(EntityRegistry* entity_registry);

    void add(TypeId component, Event event,
             std::function<void(Entity)> callback);

    void event(TypeId component, EntityId entity, Event event);

   private:
    std::unordered_map<
        Event,
        std::unordered_map<TypeId, std::vector<std::function<void(Entity)>>>>
        callbacks;

    EntityRegistry* entity_registry;
};

#endif  // OBSERVER_DISPATCHER_H