#include "EntityManager.h"

void ECS::EntityManager::Destroy() {
    for (auto entityContainer : m_entityContainers) {
        delete (entityContainer.second);
    }
}

void ECS::EntityManager::DestroyEntity(EntityId id) {
    IEntity* entity = GetEntity(id);
    const TypeId typeId = entity->GetEntityTypeId();

    auto container = m_entityContainers.find(typeId);
    if (container != m_entityContainers.end()) {
        container->second->DestroyEntity(entity);
    }
}

ECS::IEntity* ECS::EntityManager::GetEntity(EntityId id) {
    auto entity = m_entities.find(id);
    if (entity == m_entities.end()) {
        return nullptr;
    }
    return entity->second;
}
