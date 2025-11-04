#ifndef ENTITY_REGISTRY_H
#define ENTITY_REGISTRY_H

#include <optional>
#include <unordered_map>
#include <vector>

#include "EntityId.h"

class Entity;
class ComponentRegistry;

class EntityRegistry {
   public:
    EntityRegistry();

    Entity createEntity(ComponentRegistry* world);
    void killEntity(Entity entity);

    std::optional<Entity> getEntityFromId(EntityId id);
    std::vector<Entity> getEntities() const;

   private:
    std::vector<Entity> entities;
    std::unordered_map<EntityId, size_t> entity_ids;

    EntityId next_entity_id;
};

#endif  // ENTITY_REGISTRY_H