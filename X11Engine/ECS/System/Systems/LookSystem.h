#pragma once
#include "Controls/Mouse.h"
#include "ECS/Component/Components/TransformComponent.h"
#include "ECS/System/System.h"

class LookSystem : public ECS::System<LookSystem> {
   public:
    LookSystem(TransformComponent* camera) : cam(camera) {}

    void PreUpdate() override {
        cam->SetRotation(
            quaternion(Mouse::get()->GetY(), Mouse::get()->GetX(), 0.f));
    }
    void Update() override {}
    void PostUpdate() override {}

   private:
    TransformComponent* cam;
};