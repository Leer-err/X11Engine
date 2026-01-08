#pragma once

#include "Entity.h"
#include "IScript.h"

namespace Scripts::Player {

class PlayerLookX : public IScript {
    virtual void update(Entity entity, float delta_time) override;
};

class CameraLookY : public IScript {
    virtual void update(Entity entity, float delta_time) override;
};

};  // namespace Scripts::Player