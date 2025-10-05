#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <cstddef>
#include <iterator>
#include <vector>

#include "EntityId.h"

class IComponentPool {
   public:
    virtual bool has(EntityId entity) const = 0;
    virtual void remove(EntityId entity) = 0;
};

template <typename ComponentType>
class ComponentPool : public IComponentPool {
   public:
    void add(EntityId entity) {
        auto it = std::find(entity_ids.begin(), entity_ids.end(), entity);
        if (it == entity_ids.end()) {
            entity_ids.push_back(entity);
            components.push_back({});
            return;
        }

        size_t index = std::distance(entity_ids.begin(), it);
        components[index] = {};
    }

    void set(EntityId entity, const ComponentType& component) {
        auto it = std::find(entity_ids.begin(), entity_ids.end(), entity);
        if (it == entity_ids.end()) {
            entity_ids.push_back(entity);
            components.push_back(component);
            return;
        }

        size_t index = std::distance(entity_ids.begin(), it);
        components[index] = component;
    }

    void remove(EntityId entity) override {
        // TODO: Fix pointer invalidation on remove

        auto it = std::find(entity_ids.begin(), entity_ids.end(), entity);
        if (it == entity_ids.end()) return;

        size_t index = std::distance(entity_ids.begin(), it);
        std::swap(entity_ids[index], entity_ids.back());
        std::swap(components[index], components.back());

        entity_ids.pop_back();
        components.pop_back();
    }

    const ComponentType* get(EntityId entity) const {
        auto it = std::find(entity_ids.begin(), entity_ids.end(), entity);
        if (it == entity_ids.end()) return nullptr;

        size_t index = std::distance(entity_ids.begin(), it);
        return &components[index];
    }

    bool has(EntityId entity) const override {
        auto it = std::find(entity_ids.begin(), entity_ids.end(), entity);
        return it != entity_ids.end();
    }

   private:
    std::vector<EntityId> entity_ids;
    std::vector<ComponentType> components;
};

#endif  // COMPONENT_POOL_H