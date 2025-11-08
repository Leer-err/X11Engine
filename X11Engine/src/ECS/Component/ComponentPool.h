#ifndef COMPONENT_POOL_H
#define COMPONENT_POOL_H

#include <algorithm>
#include <cstddef>
#include <deque>
#include <unordered_map>
#include <vector>

#include "EntityId.h"

struct ComponentId {
    uint32_t id;
};

constexpr ComponentId INVALID_COMPONENT_ID = {0};

class IComponentPool {
   public:
    virtual ComponentId add(EntityId entity) = 0;
    virtual bool has(EntityId entity) const = 0;
    virtual void remove(EntityId entity) = 0;
};

template <typename ComponentType>
class ComponentPool : public IComponentPool {
   public:
    ComponentPool() : next_component_id(1) {}

    ComponentId add(EntityId entity) override {
        auto it = entity_ids.find(entity);
        if (it != entity_ids.end()) return entity_components[entity];

        ComponentId component = {next_component_id++};
        size_t index = allocateIndex();
        if (index == components.size())
            components.resize(components.size() + 1);

        entity_components.emplace(entity, component);
        entity_ids.emplace(entity, index);
        component_ids.emplace(component.id, index);

        return component;
    }

    ComponentId set(EntityId entity, const ComponentType& component) {
        ComponentId id = add(entity);

        size_t index = component_ids[id.id];
        components[index] = component;

        return id;
    }

    void set(ComponentId id, const ComponentType& component) {
        auto it = component_ids.find(id.id);

        if (it == component_ids.end()) return;

        size_t index = component_ids[id.id];
        components[index] = component;
    }

    void remove(EntityId entity) override {
        auto it = entity_ids.find(entity);
        if (it == entity_ids.end()) return;

        ComponentId id = entity_components[entity];
        size_t index = entity_ids[entity];
        freeIndex(index);

        entity_components.erase(entity);
        entity_ids.erase(entity);
        component_ids.erase(id.id);
    }

    ComponentId getId(EntityId entity) const {
        auto it = entity_components.find(entity);
        if (it == entity_components.end()) return INVALID_COMPONENT_ID;

        return {it->second};
    }

    ComponentType* get(ComponentId component_id) {
        auto it = component_ids.find(component_id.id);
        if (it == component_ids.end()) return nullptr;

        size_t index = it->second;
        return &components[index];
    }

    ComponentType* get(EntityId entity) {
        auto it = entity_ids.find(entity);
        if (it == entity_ids.end()) return nullptr;

        size_t index = it->second;
        return &components[index];
    }

    bool has(EntityId entity) const override {
        return entity_ids.find(entity) != entity_ids.end();
    }

   private:
    size_t allocateIndex() {
        if (empty_chunks.empty()) return components.size();

        size_t index;

        auto& chunk = empty_chunks[0];
        index = chunk.begin++;

        if (chunk.begin == chunk.end + 1) {
            empty_chunks.pop_front();
        }

        return index;
    }

    void freeIndex(size_t index) {
        auto it =
            std::lower_bound(empty_chunks.begin(), empty_chunks.end(), index,
                             [](const EmptyChunk& chunk, size_t index) {
                                 return (chunk.end + 1) < index;
                             });

        if (it == empty_chunks.end()) {
            empty_chunks.emplace_back(index, index);
            return;
        }

        if ((*it).end == index - 1) {
            (*it).end = index;
            return;
        }

        if ((*it).begin == index + 1) {
            (*it).begin = index;
            return;
        }

        empty_chunks.insert(it, {index, index});
    }

    struct EmptyChunk {
        size_t begin;
        size_t end;
    };

    uint32_t next_component_id;

    std::unordered_map<EntityId, ComponentId> entity_components;
    std::unordered_map<EntityId, size_t> entity_ids;
    std::unordered_map<uint32_t, size_t> component_ids;
    std::vector<ComponentType> components;
    std::deque<EmptyChunk> empty_chunks;
};

#endif  // COMPONENT_POOL_H