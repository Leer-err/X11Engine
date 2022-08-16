#pragma once
#include <vector>

#include "Component.h"
#include "Graphics/Model.h"


using std::vector;

struct RenderComponent : public ECS::Component<RenderComponent> {
    RenderComponent(uint32_t owner, Model* model)
        : Component(owner), model(model) {}
    Model* model;
};
