#pragma once
#include "ECS/System/System.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "Controls/Mouse.h"

class LookSystem : public ECS::System<LookSystem> {
public:
	LookSystem(TransformComponent* camera) : cam(camera) {}

	void PreUpdate() override { 
		cam->rotation = quaternion(Mouse::get()->GetY(), Mouse::get()->GetX(), 0.f);
	}
	void Update() override {}
	void PostUpdate() override {}
private:
	TransformComponent* cam;
};