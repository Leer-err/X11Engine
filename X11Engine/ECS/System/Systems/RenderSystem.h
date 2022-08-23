#pragma once

#include "ECS/Entity/IEntity.h"
#include "Graphics/Camera.h"
#include "System.h"

class RenderSystem : public ECS::System<RenderSystem> {
   public:
    RenderSystem() : cameraEntity(0) {}

    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;

    void SetCamera(const EntityId camera);

   private:
    EntityId cameraEntity;
};