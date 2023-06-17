#pragma once
#include <vector>

#include "../../../Scene/Scene.h"
#include "../Component.h"
#include "Framework/Graphics/Model.h"

using std::vector;

struct RenderComponent : public ECS::Component<RenderComponent> {
    RenderComponent(uint32_t owner, Scene::Node* node, Model* model)
        : Component(owner), model(model) {
        node->SetModel(model);
    }
    Model* model;
};
