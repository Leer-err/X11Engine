#pragma once
#include <vector>

#include "Component.h"
#include "Graphics/Model.h"
#include "Scene/Scene.h"

using std::vector;

struct RenderComponent : public ECS::Component<RenderComponent> {
    RenderComponent(uint32_t owner, Scene::Node* node, Model* model)
        : Component(owner), model(model) {
        node->SetModel(model);
    }
    Model* model;
};
