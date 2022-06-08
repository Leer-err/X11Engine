#pragma once
#include "Component.h"
#include "PositionComponent.h"
#include "Matrix.h"

struct CameraComponent : ECS::Component<CameraComponent> {
	CameraComponent(EntityId owner, vector3 viewDirection, PositionComponent* position) : Component(owner),
		position(position), viewDirection(viewDirection), 
		projectionMatrix(PerspectiveProjectionMatrix(16.f / 9.f, 60, 1000.f, 0.1f)),
		viewMatrix(LookToMatrix(position->position, viewDirection, {0.f, 1.f, 0.f})) {}
	matrix projectionMatrix;
	matrix viewMatrix;
	vector3 viewDirection;
	PositionComponent* position;
};