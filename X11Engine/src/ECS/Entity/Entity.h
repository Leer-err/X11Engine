#ifndef ENTITY_H
#define ENTITY_H

#include "ComponentRegistry.h"
#include "EntityId.h"

class Entity {
   public:
    Entity(EntityId id, ComponentRegistry* component_registry)
        : id(id), component_registry(component_registry) {}

    template <typename ComponentType>
    void add() {
        component_registry->add<ComponentType>(id);
    }

    template <typename ComponentType>
    void set(const ComponentType& component) {
        component_registry->set<ComponentType>(id, component);
    }

    template <typename ComponentType>
    bool has() const {
        return component_registry->has<ComponentType>(id);
    }

    template <typename ComponentType>
    void remove() {
        component_registry->remove<ComponentType>(id);
    }

    template <typename ComponentType>
    const ComponentType* get() const {
        return component_registry->get<ComponentType>(id);
    }

    EntityId getId() const { return id; }

    bool operator==(const Entity& b) const { return this->id == b.id; };

   private:
    EntityId id;

    ComponentRegistry* component_registry;
};

#endif  // ENTITY_H