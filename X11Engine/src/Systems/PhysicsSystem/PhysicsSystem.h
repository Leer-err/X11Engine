#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include <memory>

#include "ISystem.h"
#include "PhysicsFactory.h"
#include "World.h"

class PhysicsSystem : public ISystem {
   public:
    PhysicsSystem(std::shared_ptr<PhysicsFactory> factory, World& world,
                  float step);
    ~PhysicsSystem();

    bool prepare();

    void update(World& world, float delta_time);

   private:
    void updateDirtyDynamics(World& world);
    void updateDirtyStatics(World& world);

    float fixed_step;
    float accumulated_time;

    std::shared_ptr<PhysicsFactory> factory;
};

#endif  // PHYSICS_SYSTEM_H