#pragma once
#include "Types.h"
#include "ComponentManager.h"
#include <vector>

using std::vector;

namespace ECS {
	class IEntity {
		friend class EntityManager;
	public:
		IEntity(EntityId id) : m_id(id), m_componentManager(ECS::ComponentManager::get()) {}

		template<class T, class... Args>
		T* AddComponent(Args&&... args) {
			return m_componentManager->AddComponent<T, Args...>(m_id, args...);
		}

		template<class T>
		T* GetComponent() {
			return m_componentManager->GetComponent<T>(m_id);
		}

		inline vector<EntityId>::iterator begin() {
			return m_childs.begin();
		}
		inline vector<EntityId>::iterator end() {
			return m_childs.end();
		}
		inline void MakeChild(EntityId entity) {
			m_childs.push_back(entity);
		}
		inline void RemoveChild(EntityId entity) {
			auto child = std::find(m_childs.begin(), m_childs.end(), entity);
			if(child != m_childs.end()){
				m_childs.erase(child);
			}
		}

		virtual ~IEntity() {};

		virtual const TypeId GetEntityTypeId() const = 0;

		inline const EntityId GetEntityId() const { return m_id; };
	private:
		vector<EntityId> m_childs;
		EntityId m_id;
		ComponentManager* m_componentManager;
	};
}