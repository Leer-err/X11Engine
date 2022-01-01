#pragma once
#include "IEntity.h"
#include "Memory.h"

namespace ECS {
	class EntityManager
	{
		class IEntityContainer {
		public:
			virtual ~IEntityContainer() {};

			virtual void DestroyEntity(IEntity* object) = 0;
		};

		template<class T>
		class EntityContainer : public Memory::ChunkAllocator<T, CHUNK_SIZE>, public IEntityContainer {
		public:
			EntityContainer() {};
			virtual ~EntityContainer() {};

			virtual void DestroyEntity(IEntity* object) {
				object->~IEntity();
				this->DestroyObject(object);
			}
		private:
			EntityContainer(const EntityContainer&) = delete;
			EntityContainer& operator=(const EntityContainer&) = delete;
		};
	public:
		inline static EntityManager* Get() {
			static EntityManager instance;
			return &instance;
		}

		inline void SetData(ComponentManager* componentManager) {
			m_componentManager = componentManager;
		}

		inline void Destroy() {
			for (auto entityContainer : m_entityContainers) {
				delete (entityContainer.second);
			}
		}

		template<class T, class... Args>
		EntityId CreateEntity(Args&&... args) {
			void* ptr = GetEntityContiner<T>()->CreateObject();
			EntityId id = AqcuireId();
			IEntity* entity = (IEntity*)ptr;
			entity->m_id = id;
			entity->m_componentManager = m_componentManager;

			entity = new (ptr)T(args...);

			m_entities.emplace(id, entity);
			return id;
		}
		void DestroyEntity(EntityId id)
		{
			IEntity* entity = GetEntity(id);
			const TypeId typeId = entity->GetEntityTypeId();

			auto container = m_entityContainers.find(typeId);
			if (container != m_entityContainers.end()) {
				container->second->DestroyEntity(entity);
			}
		}
		IEntity* GetEntity(EntityId id)
		{
			auto entity = m_entities.find(id);
			if (entity == m_entities.end()) {
				return nullptr;
			}
			return entity->second;
		}
	protected:
		EntityManager() : m_nextId(0) {};

		template<class T>
		EntityContainer<T>* GetEntityContiner() {
			auto containerPair = m_entityContainers.find(T::TYPE_ID);
			EntityContainer<T>* newContainer = nullptr;
			if (containerPair == m_entityContainers.end()) {
				newContainer = new EntityContainer<T>();
				m_entityContainers[T::TYPE_ID] = newContainer;
			}
			else {
				newContainer = (EntityContainer<T>*)containerPair->second;
			}
			assert(newContainer != nullptr && "FAILED TO GET ENTITY CONTAINER");
			return newContainer;
		};

		inline EntityId const AqcuireId() { return m_nextId++; };
	private:
		EntityManager(const EntityManager&) = delete;
		EntityManager& operator=(const EntityManager&) = delete;

		std::unordered_map<EntityId, IEntity*> m_entities;
		std::unordered_map<TypeId, IEntityContainer*> m_entityContainers;
		EntityId m_nextId;
		ComponentManager* m_componentManager;
	};
}