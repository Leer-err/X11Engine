#pragma once

#include <stdint.h>

namespace ECS {
class IComponent {
   public:
    IComponent(uint32_t owner) : m_owner(owner) {}
    virtual ~IComponent(){};

    virtual inline const TypeId GetComponentTypeId() const = 0;
    virtual inline const uint32_t GetOwner() const { return m_owner; }

   private:
    uint32_t m_owner;
};
}  // namespace ECS