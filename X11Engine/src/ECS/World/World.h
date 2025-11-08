#ifndef WORLD_H
#define WORLD_H

#include <optional>

#include "ComponentPool.h"
#include "ComponentRegistry.h"
#include "EntityId.h"
#include "EntityRegistry.h"
#include "QueryBuilder.h"
#include "SystemDispatcher.h"
#include "TypeIdHelper.h"

class Entity;

class World {
   public:
    World() : component_registry(), system_dispatcher(*this) {}

    Entity createEntity() {
        return entity_registry.createEntity(&component_registry);
    }

    std::optional<Entity> getEntity(EntityId id) {
        return entity_registry.getEntityFromId(id).value();
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
    void set(ComponentId id, const ComponentType& component) {
        component_registry.set<ComponentType>(id, component);
    }

    template <typename ComponentType>
    ComponentType* get(Entity entity) {
        return component_registry.get<ComponentType>(entity);
    }

    template <typename ComponentType>
    ComponentType* get(const ComponentId& component) {
        return component_registry.get<ComponentType>(component);
    }

    template <typename ComponentType>
    void remove(Entity entity) {
        return component_registry.remove<ComponentType>(entity);
    }

    QueryBuilder query() {
        return QueryBuilder(entity_registry, component_registry);
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
};

#endif  // WORLD_H