#include "ObserverDispatcher.h"

#include "Entity.h"

ObserverDispatcher::ObserverDispatcher(EntityRegistry* entity_registry)
    : entity_registry(entity_registry) {}

void ObserverDispatcher::add(TypeId component, Event event,
                             std::function<void(Entity)> callback) {
    callbacks[event][component].push_back(callback);
}

void ObserverDispatcher::event(TypeId component, EntityId entity_id,
                               Event event) {
    if (callbacks[event].find(component) == callbacks[event].end()) return;

    Entity entity = entity_registry->getEntityFromId(entity_id);

    for (const auto& callback : callbacks[event][component]) {
        callback(entity);
    }
}