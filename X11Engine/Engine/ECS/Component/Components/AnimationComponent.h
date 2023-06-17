#pragma once
#include <string>

#include "../Component.h"
#include "../ComponentManager.h"
#include "../Components/RenderComponent.h"

using std::string;

struct AnimationComponent : public ECS::Component<AnimationComponent> {
    AnimationComponent(uint32_t owner, const char* name, Model* model)
        : Component(owner), name(name), time(0.0f) {
        int animationIndex = model->animationNames.at(name);
        animation = &model->animations.at(animationIndex);
    }
    Animation* animation;
    string name;
    float time;
};
