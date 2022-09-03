#pragma once
#include <vector>

#include "ECS/Component/ComponentManager.h"
#include "Types/Types.h"

using std::forward;
using std::vector;

struct EntityId {
    EntityId(uint32_t id) : id(id) {}

    template <class T, typename... ARGS>
    inline T* AddComponent(ARGS&&... args) {
        return ECS::ComponentManager::get()->AddComponent<T>(
            id, forward<ARGS>(args)...);
    }

    template <class T>
    inline void RemoveComponent() {
        ECS::ComponentManager::get()->RemoveComponent<T>(id);
    }

    template <class T>
    inline T* GetComponent() {
        return ECS::ComponentManager::get()->GetComponent<T>(id);
    }

    inline operator uint32_t() const { return id; }

    uint32_t id;
};

namespace ECS {
class Entity {
    friend class EntityManager;

   public:
    Entity(EntityId id) : m_id(id) {}

    virtual ~Entity(){};

    inline const EntityId GetEntityId() const { return m_id; };

   private:
    EntityId m_id;
};
}  // namespace ECS