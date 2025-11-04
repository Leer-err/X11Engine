#pragma once

#include "ISystem.h"

struct DeleteProjectile {};

class DeleteProjectileSystem : public ISystem {
   public:
    bool prepare(World& world) override;
    void update(World& world, float delta_time) override;
};