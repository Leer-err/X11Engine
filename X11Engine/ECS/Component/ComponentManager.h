#pragma once
#include <unordered_map>
#include <utility>
#include "Component.h"
#include "Memory.h"

using std::unordered_map;
using std::vector;
using std::forward;

namespace ECS {
	class ComponentManager
	{
		class IComponentContainer {
		public:
			virtual ~IComponentContainer() {};

			virtual void DestroyComponent(IComponent* component) = 0;
		};

		template<class T>
		class ComponentContainer : public Memory::ChunkAllocator<T, CHUNK_SIZE>, public IComponentContainer {
		public:
			ComponentContainer() : Memory::ChunkAllocator<T, CHUNK_SIZE>() {}
			virtual ~ComponentContainer() {}

			virtual void DestroyComponent(IComponent* component) override {
				component->~IComponent();
				this->DestroyObject(component);
			}
		};
	public:
		inline static ComponentManager* Get() {
			static ComponentManager instance;
			return &instance;
		}

		template<class T, class... Args>
		T* const AddComponent(const EntityId entityId, Args&&... args)
		{
			void* ptr = GetComponentContainer<T>()->CreateObject();

			IComponent* component = new (ptr)T(entityId, forward<Args>(args)...);

			auto componentList = m_lookupTable.find(entityId);
			if (componentList == m_lookupTable.end()) {
				vector<IComponent*> newComponents(Helper<IComponent>::Get(), nullptr);
				newComponents[T::TYPE_ID] = component;
				m_lookupTable.emplace(entityId, newComponents);
			}
			else {
				componentList->second[T::TYPE_ID] = component;
			}

			return (T*)component;
		}

		template<class T>
		T* const GetComponent(const EntityId entityId) const
		{
			auto entity = m_lookupTable.find(entityId);
			IComponent* component = entity->second[T::TYPE_ID];

			return (T*)component;
		}

		template<class T>
		void RemoveComponent(EntityId entityId)
		{
			auto entity = m_lookupTable.find(entityId);
			IComponent* component = entity->second[T::TYPE_ID];

			entity->second[T::TYPE_ID] = nullptr;

			GetComponentContainer<T>()->DestroyObject(component);
		}

		void RemoveAllComponents(EntityId entityId);
		void Destroy();

		template<class T>
		inline typename ComponentContainer<T>::iterator begin()
		{
			ComponentContainer<T>* c = GetComponentContainer<T>();
			return c->begin();
		}

		template<class T>
		inline typename ComponentContainer<T>::iterator end()
		{
			return GetComponentContainer<T>()->end();
		}
	protected:
		ComponentManager() : m_componentContainers() {}

		template<class T>
		ComponentContainer<T>* GetComponentContainer() {
			auto containerPair = m_componentContainers.find(T::TYPE_ID);
			ComponentContainer<T>* container = nullptr;
			if (containerPair == m_componentContainers.end()) {
				container = new ComponentContainer<T>();
				m_componentContainers.emplace(T::TYPE_ID, container);
			}
			else {
				container = dynamic_cast<ComponentContainer<T>*>(containerPair->second);
			}
			return container;
		}
	private:
		ComponentManager(ComponentManager&) = delete;
		ComponentManager& operator=(ComponentManager&) = delete;

		unordered_map<TypeId, IComponentContainer*> m_componentContainers;
		unordered_map<EntityId, std::vector<IComponent*>> m_lookupTable;
	};
}