#include "ComponentManager.h"

inline void ECS::ComponentManager::Destroy() {
    for (auto componentContainer : m_componentContainers) {
        delete (componentContainer.second);
    }
}

void ECS::ComponentManager::RemoveAllComponents(uint32_t entityId) {
    auto entity = m_lookupTable.find(entityId);
    for (int i = 0; i < entity->second.size(); i++) {
        IComponent* component = entity->second[i];

        if (component == nullptr) {
            continue;
        }

        m_componentContainers.find(i)->second->DestroyComponent(component);
    }
    m_lookupTable.erase(entityId);
}
