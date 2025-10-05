#ifndef WORLD_H
#define WORLD_H

#include "ComponentRegistry.h"
#include "EntityRegistry.h"
#include "ObserverBuilder.h"
#include "ObserverDispatcher.h"
#include "QueryBuilder.h"
#include "SystemDispatcher.h"
#include "TypeIdHelper.h"

class Entity;

class World {
   public:
    World()
        : observer_dispatcher(&entity_registry),
          component_registry(&observer_dispatcher) {}

    Entity createEntity() {
        return entity_registry.createEntity(&component_registry);
    }

    void killEntity(Entity entity) {
        entity_registry.killEntity(entity);
        component_registry.removeAll(entity.getId());
    }

    template <typename ComponentType>
    void add(Entity entity) {
        component_registry.add<ComponentType>(entity);
    }

    template <typename ComponentType>
    void set(Entity entity, const ComponentType& component) {
        component_registry.set<ComponentType>(entity, component);
    }

    template <typename ComponentType>
    const ComponentType* get(Entity entity) const {
        return component_registry.get<ComponentType>(entity);
    }

    template <typename ComponentType>
    void remove(Entity entity) {
        return component_registry.remove<ComponentType>(entity);
    }

    QueryBuilder query() {
        return QueryBuilder(entity_registry, component_registry);
    }

    template <typename ComponentType>
    ObserverBuilder observer() {
        TypeId type_id = TypeIdHelper::getTypeId<ComponentType>();

        return ObserverBuilder(type_id, observer_dispatcher);
    }

    void progress(float delta_time) {
        system_dispatcher.progress(*this, delta_time);
    }

    template <typename System, typename... Args>
    SystemDispatcher::SystemDescription addSystem(Args&&... args) {
        return system_dispatcher.add<System>(std::forward<Args>(args)...);
    }

   private:
    EntityRegistry entity_registry;
    ComponentRegistry component_registry;
    SystemDispatcher system_dispatcher;
    ObserverDispatcher observer_dispatcher;
};

#endif  // WORLD_H