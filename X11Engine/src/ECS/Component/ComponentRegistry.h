#ifndef COMPONENT_REGISTRY_H
#define COMPONENT_REGISTRY_H

#include <memory>
#include <tracy/Tracy.hpp>
#include <unordered_map>

#include "ComponentPool.h"
#include "TypeId.h"
#include "TypeIdHelper.h"

class ComponentRegistry {
   public:
    template <typename ComponentType>
    void add(EntityId entity) {
        ComponentPool<ComponentType>* pool = getOrCreatePool<ComponentType>();

        pool->add(entity);

        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();
    }

    template <typename ComponentType>
    void set(EntityId entity, ComponentType&& component) {
        ZoneScoped;
        ComponentPool<ComponentType>* pool = getOrCreatePool<ComponentType>();

        pool->set(entity, std::forward<ComponentType>(component));

        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();
    }

    template <typename ComponentType>
    void set(ComponentId id, ComponentType&& component) {
        ZoneScoped;
        ComponentPool<ComponentType>* pool = getOrCreatePool<ComponentType>();

        pool->set(id, std::forward<ComponentType>(component));
    }

    template <typename ComponentType>
    ComponentType* get(EntityId entity) {
        ZoneScoped;
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return nullptr;

        return pool->get(entity);
    }

    template <typename ComponentType>
    const ComponentType* get(EntityId entity) const {
        ZoneScoped;
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return nullptr;

        return pool->get(entity);
    }

    template <typename ComponentType>
    ComponentType* get(ComponentId component_id) {
        ZoneScoped;
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return nullptr;

        return pool->get(component_id);
    }

    template <typename ComponentType>
    ComponentId getComponentId(EntityId entity) const {
        const ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return INVALID_COMPONENT_ID;

        return pool->getId(entity);
    }

    ComponentId getComponentId(EntityId entity, TypeId component_type) const {
        const IComponentPool* pool = getPoolById(component_type);

        if (pool == nullptr) return INVALID_COMPONENT_ID;

        return pool->getId(entity);
    }

    template <typename ComponentType>
    void remove(EntityId entity) {
        ComponentPool<ComponentType>* pool = getPool<ComponentType>();

        if (pool == nullptr) return;

        pool->remove(entity);
    }

    void remove(EntityId entity, TypeId component_type) {
        IComponentPool* pool = getPoolById(component_type);

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

    bool has(EntityId entity, TypeId component_type) const {
        const IComponentPool* pool = getPoolById(component_type);

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
        auto it = component_pools.find(component_id);
        if (it == component_pools.end()) return nullptr;

        return it->second.get();
    }

    template <typename ComponentType>
    ComponentPool<ComponentType>* getPool() const {
        size_t type_id = TypeIdHelper::getTypeId<ComponentType>();

        return static_cast<ComponentPool<ComponentType>*>(getPoolById(type_id));
    }

    std::unordered_map<TypeId, std::unique_ptr<IComponentPool>> component_pools;
};

#endif  // COMPONENT_REGISTRY_H