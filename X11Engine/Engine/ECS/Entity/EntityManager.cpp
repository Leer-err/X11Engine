#include "EntityManager.h"

void ECS::EntityManager::Destroy() { delete (m_entityContainer); }

void ECS::EntityManager::DestroyEntity(EntityId id) {
    Entity* entity = GetEntity(id);

    m_entityContainer->DestroyEntity(entity);
}

ECS::Entity* ECS::EntityManager::GetEntity(EntityId id) {
    auto entity = m_entities.find(id);
    if (entity == m_entities.end()) {
        return nullptr;
    }
    return entity->second;
}
