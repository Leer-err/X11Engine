#include "QueryBuilder.h"

#include <tracy/Tracy.hpp>
#include <vector>

#include "ComponentRegistry.h"
#include "Entity.h"
#include "EntityRegistry.h"
#include "Parent.h"

QueryBuilder::QueryBuilder(const EntityRegistry& entity_registry,
                           const ComponentRegistry& component_registry)
    : entity_registry(entity_registry),
      component_registry(component_registry),
      search_parent(false) {}

std::vector<Entity> QueryBuilder::execute() const {
    ZoneScoped;

    auto entities = entity_registry.getEntities();

    std::vector<Entity> result;
    for (const auto& entity : entities) {
        bool match = true;

        for (const auto& required_component : required) {
            if (!component_registry.has(entity.getId(), required_component)) {
                match = false;
                break;
            }
        }
        if (!match) continue;

        for (const auto& excluded_component : excluded) {
            if (component_registry.has(entity.getId(), excluded_component)) {
                match = false;
                break;
            }
        }
        if (!match) continue;

        if (search_parent) {
            const Parent* parent =
                component_registry.get<Parent>(entity.getId());

            if (parent->parent.getId() != parent_id) {
                continue;
            }
        }

        result.push_back(entity);
    }

    return result;
}

QueryBuilder& QueryBuilder::childOf(Entity parent) {
    search_parent = true;
    parent_id = parent.getId();

    with<Parent>();

    return *this;
}

QueryBuilder& QueryBuilder::filter(const FilterFunction& filter) {
    filters.push_back(filter);
    return *this;
}
