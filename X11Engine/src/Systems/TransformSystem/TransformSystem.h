#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "ISystem.h"

class TransformSystem : public ISystem {
   public:
    TransformSystem(World& world);

    bool prepare() override;

    void update(World& world, float delta_time) override;
};

#endif  // TRANSFORM_SYSTEM_H