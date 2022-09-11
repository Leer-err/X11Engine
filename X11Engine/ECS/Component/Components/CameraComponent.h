#pragma once
#include "ECS/Component/Component.h"
#include "Graphics/Camera.h"

struct CameraComponent : ECS::Component<CameraComponent> {
    CameraComponent(uint32_t owner, float farZ, float nearZ, float fov,
                    float aspectRatio, const vector3& forward = {0.f, 0.f, 1.f},
                    const vector3& up = {0.f, 1.f, 0.f})
        : Component(owner),
          camera(farZ, nearZ, fov, aspectRatio, forward, up) {}
    Camera camera;
};