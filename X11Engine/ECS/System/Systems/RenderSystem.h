#pragma once

#include "ECS/Entity/Entity.h"
#include "ECS/System/System.h"
#include "Graphics/Camera.h"

class RenderSystem : public ECS::System<RenderSystem> {
   public:
    RenderSystem() : cameraEntity(0) {}

    void PreUpdate() override;
    void Update(float delta) override;
    void PostUpdate() override;

    void SetCamera(const EntityId camera);

   private:
    EntityId cameraEntity;
};