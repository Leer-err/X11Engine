#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <vector>

#include "ComponentRegistry.h"
#include "Entity.h"
#include "EntityId.h"
#include "EntityRegistry.h"
#include "TypeId.h"
#include "TypeIdHelper.h"

class World;

class QueryBuilder {
    // TODO: Query caching

    friend class World;

   protected:
    QueryBuilder(const EntityRegistry& entity_registry,
                 const ComponentRegistry& component_registry);

   public:
    template <typename ComponentType>
    QueryBuilder& with() {
        TypeId id = TypeIdHelper::getTypeId<ComponentType>();

        required.push_back(id);

        return *this;
    }

    template <typename ComponentType>
    QueryBuilder& without() {
        TypeId id = TypeIdHelper::getTypeId<ComponentType>();

        excluded.push_back(id);

        return *this;
    }

    QueryBuilder& childOf(Entity parent);

    std::vector<Entity> execute() const;

   private:
    std::vector<TypeId> required;
    std::vector<TypeId> excluded;

    bool search_parent;
    EntityId parent_id;

    const EntityRegistry& entity_registry;
    const ComponentRegistry& component_registry;
};

#endif  // QUERY_BUILDER_H