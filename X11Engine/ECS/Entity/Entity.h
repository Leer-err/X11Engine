#pragma once
#include "IEntity.h"

namespace ECS {
template <class E>
class Entity : public IEntity {
   public:
    Entity(EntityId id) : IEntity(id) {}
    virtual ~Entity(){};

    inline const TypeId GetEntityTypeId() const override { return TYPE_ID; };

    static const TypeId TYPE_ID;

   private:
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;
};

template <class E>
const TypeId Entity<E>::TYPE_ID = Helper<IEntity>::Get<E>();
}  // namespace ECS