#pragma once
#include "Component.h"
#include "Types/Vector3.h"

struct PositionComponent : public ECS::Component<PositionComponent> {
	PositionComponent(EntityId owner, vector3 pos) : Component(owner), position(pos) {}
	vector3 position;
};