#ifndef PRE_SIMULATE_UPDATE_SYSTEM_H
#define PRE_SIMULATE_UPDATE_SYSTEM_H

#include "ISystem.h"
#include "World.h"

class PreSimulateUpdateSystem : public ISystem {
   public:
    bool prepare() override;
    void update(World& world, float delta_time) override;
};

#endif  // PRE_SIMULATE_UPDATE_SYSTEM_H