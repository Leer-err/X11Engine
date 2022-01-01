#pragma once
#include "Types.h"

namespace ECS {
	class IComponent {
	public:
		virtual ~IComponent() {};

		virtual inline const TypeId GetComponentTypeId() const = 0;
		inline bool const IsActive() const { return m_enabled; }
		inline void SetActive(bool state) { m_enabled = state; }

		EntityId m_owner;
	protected:
		bool m_enabled;
	};
}