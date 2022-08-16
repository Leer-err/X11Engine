#pragma once
#include <unordered_map>

#include "IEntity.h"
#include "Memory.h"

using std::forward;
using std::unordered_map;

namespace ECS {
class EntityManager {
    class IEntityContainer {
       public:
        virtual ~IEntityContainer(){};

        virtual void DestroyEntity(IEntity* object) = 0;
    };

    template <class T>
    class EntityContainer : public Memory::ChunkAllocator<T, CHUNK_SIZE>,
                            public IEntityContainer {
       public:
        EntityContainer(){};
        virtual ~EntityContainer(){};

        virtual void DestroyEntity(IEntity* object) {
            object->~IEntity();
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

    template <class T, class... Args>
    EntityId CreateEntity(Args&&... args) {
        void* ptr = GetEntityContiner<T>()->CreateObject();
        EntityId id = AqcuireId();
        IEntity* entity = (IEntity*)ptr;
        entity->m_id = id;

        entity = new (ptr) T(id, forward<Args>(args)...);

        m_entities.emplace(id, entity);
        return id;
    }
    void DestroyEntity(EntityId id);
    IEntity* GetEntity(EntityId id);

   protected:
    EntityManager() : m_nextId(0) {}

    template <class T>
    EntityContainer<T>* GetEntityContiner() {
        auto containerPair = m_entityContainers.find(T::TYPE_ID);
        EntityContainer<T>* newContainer = nullptr;
        if (containerPair == m_entityContainers.end()) {
            newContainer = new EntityContainer<T>();
            m_entityContainers[T::TYPE_ID] = newContainer;
        } else {
            newContainer = (EntityContainer<T>*)containerPair->second;
        }
        return newContainer;
    };

    inline EntityId const AqcuireId() { return m_nextId++; };

   private:
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;

    unordered_map<uint32_t, IEntity*> m_entities;
    unordered_map<TypeId, IEntityContainer*> m_entityContainers;
    uint32_t m_nextId;
};
}  // namespace ECS