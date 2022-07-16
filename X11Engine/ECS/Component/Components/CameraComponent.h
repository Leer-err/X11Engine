#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Matrix.h"

struct CameraComponent : ECS::Component<CameraComponent> {
	CameraComponent(EntityId owner, vector3 viewDirection, TransformComponent* transform) : Component(owner),
		transform(transform), viewDirection(viewDirection) {}
	vector3 viewDirection;
	TransformComponent* transform;
};