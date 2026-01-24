#pragma once

#include "Entity.h"
#include "IScript.h"

namespace Scripts::Player {

class PlayerLookX : public IScript {
   public:
    PlayerLookX() : yaw(0) {}

    virtual void update(Entity entity, float delta_time) override;

   private:
    float yaw;
};

class CameraLookY : public IScript {
   public:
    CameraLookY() : pitch(0) {}

    virtual void update(Entity entity, float delta_time) override;

   private:
    float pitch;
};

};  // namespace Scripts::Player