#include "EntityRegistry.h"

#include <cstddef>

#include "ComponentRegistry.h"
#include "Entity.h"
#include "EntityId.h"

constexpr size_t FIRST_ENTITY_ID = 1;

EntityRegistry::EntityRegistry() : next_entity_id(FIRST_ENTITY_ID) {}

Entity EntityRegistry::createEntity(ComponentRegistry* world) {
    EntityId id = next_entity_id++;
    entity_ids.emplace(id, entities.size());

    auto entity = entities.emplace_back(id, world);

    return entity;
}

void EntityRegistry::killEntity(Entity entity) {
    EntityId remove_id = entity.getId();

    auto entity_it = entity_ids.find(remove_id);
    if (entity_it == entity_ids.end()) return;
    size_t remove_index = entity_it->second;

    size_t replace_index = entities.size() - 1;
    EntityId replace_id = entities[remove_id].getId();

    std::swap(entities[remove_index], entities[replace_index]);
    entities.erase(entities.end() - 1);

    entity_ids[replace_id] = remove_index;
    entity_ids.erase(remove_id);
}

std::vector<Entity> EntityRegistry::getEntities() const { return entities; }

Entity EntityRegistry::getEntityFromId(EntityId id) {
    size_t index = entity_ids.at(id);

    return entities[index];
}
