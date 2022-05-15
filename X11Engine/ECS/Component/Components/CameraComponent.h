#pragma once
#include "Component.h"
#include "PositionComponent.h"
#include "Matrix.h"

struct CameraComponent : ECS::Component<CameraComponent> {
	CameraComponent(EntityId owner, PositionComponent* position) : Component(owner),
		position(position), projectionMatrix(PerspectiveProjectionMatrix(16.f/9.f, 60, 1000.f, 0.1f)) {}
	matrix projectionMatrix;
	PositionComponent* position;
};