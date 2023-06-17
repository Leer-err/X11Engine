#pragma once

#include "../../Component/Components/AnimationComponent.h"
#include "../System.h"

class AnimationSystem : public ECS::System<AnimationSystem> {
   public:
    AnimationSystem() {}

    void PreUpdate() override {}
    void Update(float delta) override {
        auto componentManager = ECS::ComponentManager::get();

        vector<uint32_t> ids;

        for (auto& animation = componentManager->begin<AnimationComponent>();
             animation != componentManager->end<AnimationComponent>();
             ++animation) {
            animation->time += delta;
            if (animation->time > animation->animation->m_duration /
                                      animation->animation->m_ticksPerSecond) {
                ids.push_back(animation->GetOwner());
            }
        }

        for (const auto& id : ids) {
            componentManager->RemoveComponent<AnimationComponent>(id);
        }
    }
    void PostUpdate() override {}
};