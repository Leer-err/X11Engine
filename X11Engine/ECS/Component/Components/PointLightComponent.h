#pragma once
#include "Component.h"
#include "Graphics/Light.h"

struct vector3;

struct PointLightComponent : public ECS::Component<PointLightComponent> {
	PointLightComponent(EntityId owner, PointLight light) : Component(owner), light(light) {}
	PointLight light;
};