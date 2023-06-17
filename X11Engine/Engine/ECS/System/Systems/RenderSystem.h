#pragma once

#include "../../Entity/Entity.h"
#include "../System.h"
#include "Framework/Graphics/Camera.h"

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