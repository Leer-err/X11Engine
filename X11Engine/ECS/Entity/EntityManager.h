#pragma once
#include <unordered_map>

#include "Entity.h"
#include "Memory/Memory.h"

using std::forward;
using std::unordered_map;

namespace ECS {
class EntityManager {
    class EntityContainer : public Memory::ChunkAllocator<Entity, CHUNK_SIZE> {
       public:
        EntityContainer(){};
        virtual ~EntityContainer(){};

        virtual void DestroyEntity(Entity* object) {
            object->~Entity();
            this->DestroyObject(object);
        }

       private:
        EntityContainer(const EntityContainer&) = delete;
        EntityContainer& operator=(const EntityContainer&) = delete;
    };

   public:
    inline static EntityManager* get() {
        static EntityManager instance;
        return &instance;
    }

    void Destroy();

    EntityId CreateEntity() {
        void* ptr = m_entityContainer->CreateObject();
        EntityId id = AqcuireId();
        Entity* entity = new (ptr) Entity(id);

        m_entities.emplace(id, entity);
        return id;
    }
    void DestroyEntity(EntityId id);
    Entity* GetEntity(EntityId id);

   protected:
    EntityManager() : m_nextId(0) { m_entityContainer = new EntityContainer(); }

    inline EntityId const AqcuireId() { return m_nextId++; };

   private:
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    unordered_map<uint32_t, Entity*> m_entities;
    EntityContainer* m_entityContainer;
    uint32_t m_nextId;
};
}  // namespace ECS