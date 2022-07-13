#pragma once
#include "System.h"

class RenderSystem : public ECS::System<RenderSystem>
{
public:
	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;
};