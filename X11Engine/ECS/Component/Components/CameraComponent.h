#pragma once
#include "Component.h"
#include "Matrix.h"

struct CameraComponent : ECS::Component<CameraComponent> {
    CameraComponent(uint32_t owner, vector3 viewDirection)
        : Component(owner), viewDirection(viewDirection) {}
    vector3 viewDirection;
};