#pragma once
#include "Types/Types.h"

namespace ECS {
class IComponent {
   public:
    IComponent(EntityId owner) : m_owner(owner) {}
    virtual ~IComponent(){};

    virtual inline const TypeId GetComponentTypeId() const = 0;
    virtual inline const EntityId GetOwner() const { return m_owner; }

   private:
    EntityId m_owner;
};
}  // namespace ECS