#pragma once
#include "Types/Types.h"

namespace ECS {
class ISystem {
   public:
    virtual ~ISystem(){};

    virtual inline const TypeId GetSystemTypeId() const = 0;
    void SetPriority(uint16_t priority) { m_priority = priority; }

    virtual void PreUpdate() = 0;
    virtual void Update(float delta) = 0;
    virtual void PostUpdate() = 0;

    uint16_t m_priority;

   private:
    uint32_t m_timeSinceLastUpdate;
    uint32_t m_updateInterval;
};
}  // namespace ECS