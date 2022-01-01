#pragma once
#include "ISystem.h"
#include "Utility.h"

namespace ECS {
	template<class T>
	class System : public ISystem {
	public:
		virtual ~System() {};

		inline const TypeId GetSystemTypeId() const override { return TYPE_ID; }

		static const TypeId TYPE_ID;
	};

	template<class T>
	const TypeId System<T>::TYPE_ID = Utility<ISystem>::Get<T>();
}