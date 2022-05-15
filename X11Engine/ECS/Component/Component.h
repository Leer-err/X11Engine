#pragma once
#include "Helper.h"
#include "IComponent.h"

namespace ECS {
	template<class T>
	class Component : public IComponent {
	public:
		Component(EntityId owner) : IComponent(owner) {}
		virtual ~Component() {};

		inline const TypeId GetComponentTypeId() const override { return TYPE_ID; }

		static const TypeId TYPE_ID;
	private:
		void operator delete(void*) = delete;
		void operator delete[](void*) = delete;
	};

	template<class T>
	const TypeId Component<T>::TYPE_ID = Helper<IComponent>::Get<T>();
}