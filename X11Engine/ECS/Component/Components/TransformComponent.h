#pragma once
#include "Component.h"
#include "Types/Vector3.h"
#include "Types/Quaternion.h"

struct TransformComponent : public ECS::Component<TransformComponent> {
	TransformComponent(EntityId owner, const vector3& position, const vector3& rotation, const vector3& scale = { 1.f, 1.f, 1.f }) :
		Component(owner), position(position), rotation(rotation), scale(scale) {}
	TransformComponent(EntityId owner, const vector3& position = {}, const quaternion& rotation = {}, const vector3& scale = { 1.f, 1.f, 1.f }) :
		Component(owner), position(position), rotation(rotation), scale(scale) {}

	vector3 position;
	vector3 scale;
	quaternion rotation;
};