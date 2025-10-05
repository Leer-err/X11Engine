#ifndef COMPONENT_REGISTRY_H
#define COMPONENT_REGISTRY_H

#include <memory>
#include <unordered_map>

#include "ComponentPool.h"
#include "ObserverDispatcher.h"
#include "TypeId.h"
#include "TypeIdHelper.h"

class ComponentRegistry {
   public:
    ComponentRegistry(ObserverDispatcher* dispatcher)
        : event_dispatcher(dispatcher) {}

    template <typename ComponentType>
    void add(EntityId entity) {
        ComponentPool<ComponentType>* pool = getOrCreatePool<ComponentType>();

        pool->add(entity);

        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();
        event_dispatcher->event(type_id, entity, Event::Add);
    }

    template <typename ComponentType>
    void set(EntityId entity, const ComponentType& component) {
        ComponentPool<ComponentType>* pool = getOrCreatePool<ComponentType>();

        pool->set(entity, component);

        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();
        event_dispatcher->event(type_id, entity, Event::Set);
    }

    template <typename ComponentType>
    const ComponentType* get(EntityId entity) const {
        const ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return nullptr;

        return pool->get(entity);
    }

    template <typename ComponentType>
    void remove(EntityId entity) {
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return;

        pool->remove(entity);
    }

    void removeAll(EntityId entity) {
        for (auto& [component_type, pool] : component_pools) {
            pool->remove(entity);
        }
    }

    template <typename ComponentType>
    bool has(EntityId entity) const {
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return false;

        return pool->has(entity);
    }

    bool has(EntityId entity, TypeId component_id) const {
        const IComponentPool* pool = getPoolById(component_id);

        if (pool == nullptr) return false;

        return pool->has(entity);
    }

   private:
    template <typename ComponentType>
    ComponentPool<ComponentType>* getOrCreatePool() {
        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();

        if (component_pools.find(type_id) == component_pools.end()) {
            component_pools[type_id] =
                std::make_unique<ComponentPool<ComponentType>>();
        }

        return static_cast<ComponentPool<ComponentType>*>(
            component_pools[type_id].get());
    }

    IComponentPool* getPoolById(TypeId component_id) const {
        if (component_pools.find(component_id) == component_pools.end()) {
            return nullptr;
        }

        return component_pools.at(component_id).get();
    }

    template <typename ComponentType>
    ComponentPool<ComponentType>* getPool() const {
        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();

        if (component_pools.find(type_id) == component_pools.end()) {
            return nullptr;
        }

        return static_cast<ComponentPool<ComponentType>*>(getPoolById(type_id));
    }

    std::unordered_map<TypeId, std::unique_ptr<IComponentPool>> component_pools;

    ObserverDispatcher* event_dispatcher;
};

#endif  // COMPONENT_REGISTRY_H