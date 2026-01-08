#pragma once

#include "World.h"

class IScript {
   public:
    virtual void update(Entity entity, float delta_time) = 0;
};