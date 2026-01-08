#include "PlayerLook.h"

#include "GameInputContext.h"
#include "Quaternion.h"
#include "Transform.h"

namespace Scripts::Player {

void PlayerLookX::update(Entity entity, float delta_time) {
    Transform* transform = entity.get<Transform>();

    auto yaw = GameInputContext::get().getAxis(LOOK_X);
    auto rotation = transform->getOrientation() * Quaternion(0, yaw, 0);

    transform->setOrientation(rotation);
}

void CameraLookY::update(Entity entity, float delta_time) {
    Transform* transform = entity.get<Transform>();

    auto pitch = GameInputContext::get().getAxis(LOOK_Y);
    auto rotation = transform->getOrientation() * Quaternion(pitch, 0, 0);

    transform->setOrientation(rotation);
}

};  // namespace Scripts::Player