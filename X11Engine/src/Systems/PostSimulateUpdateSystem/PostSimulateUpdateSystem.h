#ifndef POST_SIMULATE_UPDATE_SYSTEM_H
#define POST_SIMULATE_UPDATE_SYSTEM_H

#include "ISystem.h"
#include "World.h"

class PostSimulateUpdateSystem : public ISystem {
   public:
    PostSimulateUpdateSystem(World& world);

    bool prepare() override;
    void update(World& world, float delta_time) override;
};

#endif  // POST_SIMULATE_UPDATE_SYSTEM_H