#pragma once
#include <string>

#include "ECS/Component/Component.h"

using std::string;

struct AnimationComponent : public ECS::Component<AnimationComponent> {
    AnimationComponent(uint32_t owner, char* name)
        : Component(owner), name(name), time(0.0f) {}
    string name;
    float time;
};
