#pragma once
#include "Component.h"
#include "Scene/Scene.h"

struct EntityId;

struct TransformComponent : public ECS::Component<TransformComponent> {
    TransformComponent(uint32_t owner, Node* parentNode, vector3 pos = {},
                       quaternion rotation = {},
                       vector3 scale = {1.f, 1.f, 1.f})
        : Component(owner),
          sceneNode(parentNode->AddChild(pos, scale, rotation)) {}

    inline vector3 GetPosition() const {
        return sceneNode->GetGlobalPosition();
    }
    inline vector3 GetScale() const { return sceneNode->GetScale(); }
    inline quaternion GetRotation() const { return sceneNode->GetRotation(); }
    inline matrix GetWorldMatrix() const { return sceneNode->GetWorldMatrix(); }

    inline void SetPosition(const vector3& position) {
        sceneNode->SetGlobalPosition(position);
    }
    inline void SetScale(const vector3& scale) { sceneNode->SetScale(scale); }
    inline void SetRotation(const quaternion& rotation) {
        sceneNode->SetRotation(rotation);
    }

    Node* sceneNode;
};