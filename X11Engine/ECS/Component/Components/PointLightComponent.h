#pragma once
#include "Component.h"

struct vector3;

struct PointLightComponent : public ECS::Component<PointLightComponent> {
	PointLightComponent(EntityId owner, vector3 color) : Component(owner), color(color) {}
	vector3 color;
};
