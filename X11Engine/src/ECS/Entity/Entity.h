#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <tracy/Tracy.hpp>

#include "ComponentRegistry.h"
#include "EntityId.h"

class IScript;

class Entity {
   public:
    Entity() : id(INVALID_ENTITY_ID), component_registry(nullptr) {}

    Entity(EntityId id, ComponentRegistry* component_registry)
        : id(id), component_registry(component_registry) {}

    template <typename ComponentType>
    void add() {
        component_registry->add<ComponentType>(id);
    }

    void addChild(Entity child);
    void addScript(std::unique_ptr<IScript>&& script);

    template <typename ComponentType>
    void set(ComponentType&& component) {
        component_registry->set<ComponentType>(
            id, std::forward<ComponentType>(component));
    }

    template <typename ComponentType>
    bool has() const {
        return component_registry->has<ComponentType>(id);
    }

    bool has(TypeId component_type) const {
        return component_registry->has(id, component_type);
    }

    template <typename ComponentType>
    void remove() {
        component_registry->remove<ComponentType>(id);
    }

    void remove(TypeId component_type) {
        component_registry->remove(id, component_type);
    }

    template <typename ComponentType>
    const ComponentType* get() const {
        return component_registry->get<ComponentType>(id);
    }

    template <typename ComponentType>
    ComponentType* get() {
        return component_registry->get<ComponentType>(id);
    }

    template <typename ComponentType>
    ComponentId getComponentId() const {
        return component_registry->getComponentId<ComponentType>(id);
    }

    ComponentId getComponentId(TypeId component_type) const {
        return component_registry->getComponentId(id, component_type);
    }

    bool isValid() const { return id != INVALID_ENTITY_ID; }

    EntityId getId() const { return id; }

    bool operator==(const Entity& b) const { return this->id == b.id; };

   private:
    EntityId id;

    ComponentRegistry* component_registry;
};

#endif  // ENTITY_H