#include "PlayerLook.h"

#include "GameInputContext.h"
#include "Quaternion.h"
#include "Transform.h"

namespace Scripts::Player {

constexpr float sensetivity = 10.f;
constexpr float HALF_PI = 1.57f;
constexpr float TWO_PI = 6.28f;

void PlayerLookX::update(Entity entity, float delta_time) {
    Transform* transform = entity.get<Transform>();

    yaw += GameInputContext::get().getAxis(LOOK_X) * sensetivity;

    if (yaw > TWO_PI)
        yaw -= TWO_PI;
    else if (yaw < -TWO_PI)
        yaw += TWO_PI;

    transform->setOrientation(Quaternion(0, yaw, 0));
}

void CameraLookY::update(Entity entity, float delta_time) {
    Transform* transform = entity.get<Transform>();

    pitch += GameInputContext::get().getAxis(LOOK_Y) * sensetivity;

    if (pitch > HALF_PI)
        pitch = HALF_PI;
    else if (pitch < -HALF_PI)
        pitch = -HALF_PI;

    transform->setOrientation(Quaternion(pitch, 0, 0));
}

};  // namespace Scripts::Player