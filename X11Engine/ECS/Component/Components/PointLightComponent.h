#pragma once
#include "ECS/Component/Component.h"
#include "Graphics/Light.h"

struct vector3;

struct PointLightComponent : public ECS::Component<PointLightComponent> {
    PointLightComponent(uint32_t owner, PointLight light)
        : Component(owner), light(light) {}
    PointLight light;
};