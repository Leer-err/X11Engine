#pragma once
#include <unordered_map>
#include "Component.h"
#include "Memory.h"

using std::unordered_map;
using std::vector;

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
			ComponentContainer() {}
			virtual ~ComponentContainer() {}

			virtual void DestroyComponent(IComponent* component) override {
				component->~IComponent();
				this->DestroyObject(component);
			}
		private:
			ComponentContainer(ComponentContainer&) = delete;
			ComponentContainer& operator=(ComponentContainer&) = delete;
		};
	public:
		inline static ComponentManager* Get() {
			static ComponentManager instance;
			return &instance;
		}

		inline void Destroy() {
			for (auto componentContainer : m_componentContainers) {
				delete (componentContainer.second);
			}
		}

		template<class T, class... Args>
		T* const AddComponent(const EntityId entityId, Args&&... args)
		{
			void* ptr = GetComponentContainer<T>()->CreateObject();

			reinterpret_cast<IComponent*>(ptr)->m_owner = entityId;

			IComponent* component = new (ptr)T(args...);

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

			if (component == (void*)std::numeric_limits<TypeId>::max()) {
				return nullptr;
			}

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

		void RemoveAllComponents(EntityId entityId)
		{
			auto entity = m_lookupTable.find(entityId);
			for (int i = 0; i < entity->second.size(); i++) {
				IComponent* component = entity->second[i];

				if (component == nullptr) { continue; }

				m_componentContainers.find(i)->second->DestroyComponent(component);
			}
			m_lookupTable.erase(entityId);
		}

		template<class T>
		inline typename ComponentContainer<T>::iterator begin()
		{
			return GetComponentContainer<T>()->begin();
		}

		template<class T>
		inline typename ComponentContainer<T>::iterator end()
		{
			return GetComponentContainer<T>()->end();
		}
	protected:
		ComponentManager() {
			m_componentContainers = std::unordered_map<TypeId, IComponentContainer*>();
		}

		template<class T>
		ComponentContainer<T>* GetComponentContainer() {
			auto containerPair = m_componentContainers.find(T::TYPE_ID);
			ComponentContainer<T>* newContainer = nullptr;
			if (containerPair == m_componentContainers.end()) {
				newContainer = new ComponentContainer<T>();
				m_componentContainers.emplace(T::TYPE_ID, newContainer);
			}
			else {
				newContainer = (ComponentContainer<T>*)containerPair->second;
			}
			return newContainer;
		}
	private:
		ComponentManager(ComponentManager&) = delete;
		ComponentManager& operator=(ComponentManager&) = delete;

		unordered_map<TypeId, IComponentContainer*> m_componentContainers;
		unordered_map<EntityId, std::vector<IComponent*>> m_lookupTable;
	};
}