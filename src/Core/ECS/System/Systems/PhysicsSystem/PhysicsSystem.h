#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "ISystem.h"
#include "World.h"

class PhysicsSystem : public ISystem {
   public:
    PhysicsSystem(float fixed_step);
    ~PhysicsSystem();

    void simulate(World& world, float delta_time) override;

   private:
    // void updateDirtyDynamics(World& world);
    // void updateDirtyStatics(World& world);

    float fixed_step;
    float accumulated_time;
};

#endif  // PHYSICS_SYSTEM_H