#pragma once

#include "World.h"

class IScript {
   public:
    virtual void prepare() = 0;
    virtual void update(World& world, float delta_time) = 0;
};