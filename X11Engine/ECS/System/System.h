#pragma once
#include "ISystem.h"
#include "Helper.h"

namespace ECS {
	template<class T>
	class System : public ISystem {
	public:
		virtual ~System() {};

		inline const TypeId GetSystemTypeId() const override { return TYPE_ID; }

		static const TypeId TYPE_ID;
	};

	template<class T>
	const TypeId System<T>::TYPE_ID = Helper<ISystem>::Get<T>();
}