#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Matrix.h"

struct CameraComponent : ECS::Component<CameraComponent> {
	CameraComponent(EntityId owner, vector3 viewDirection, TransformComponent* position) : Component(owner),
		position(position), viewDirection(viewDirection), 
		projectionMatrix(PerspectiveProjectionMatrix(16.f / 9.f, 60.f/180*3.14f, 1000.f, 0.1f)) {}
	matrix projectionMatrix;
	vector3 viewDirection;
	TransformComponent* position;
};