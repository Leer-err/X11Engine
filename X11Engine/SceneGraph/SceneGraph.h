#pragma once

#include <vector>

#include "Memory/Memory.h"
#include "Types/Matrix.h"
#include "Types/Quaternion.h"
#include "Types/Vector3.h"

using std::vector;

using Memory::PoolAllocator;

struct Transform {
    vector3 position;
    vector3 scale;
    quaternion rotation;

    matrix worldMatrix;
}

class Node {
   public:
    Node(vector3 pos = {0.f, 0.f, 0.f}, vector3 scale = {1.f, 1.f, 1.f},
         quaternion rotation = {}, Node* parent = nullptr)
        : m_parent(parent) {}

    inline vector3 GetLocalPosition() { return m_transform.position; }
    inline vector3 GetGlobalPosition() {
        return m_transform.position * m_transform.worldMatrix;
    }
    inline quaternion GetRotation() { return m_transform.rotation; }
    inline vector3 GetScale() { return m_transform.scale; }

    template <typename... ARGS>
    Node* AddChild(ARGS&&... args) {
        Node* node = SceneGraph::get().CreateNode(forward<ARGS>(args)...);
        m_children.push_back(node);
        return node;
    }

    void RemoveChild(Node* ptr) {
        auto& child = find(m_children.begin(), m_children.end(), ptr);

        if (child != m_children.end()) {
            m_children.erase(child);
        }
    }

    void UpdateWorldMatrix(const matrix& parentMatrix) {
        matrix localMatrix = ScalingMatrix(m_transform.scale) *
                             RotationMatrix(m_transform.rotation) *
                             TranslationMatrix(m_transform.position);
        m_transform.worldMatrix = parentMatrix * localMatrix;

        for (auto& child : children) {
            child->UpdateWorldMatrix(m_transform.worldMatrix);
        }
    }

   private:
    vector<Node*> m_children;
    Node* m_parent;

    Transform m_transform;
};

class SceneGraph {
    friend class Node;

   public:
    inline static SceneGraph& get() {
        static SceneGraph instance;
        return instance;
    }

    inline Node* GetWorldNode() { return m_world; }

   protected:
    template <typename... ARGS>
    Node* CreateNode(ARGS&&... args) {
        void* ptr = m_allocator.allocate(sizeof(Node), _alignof(Node));
        if (ptr != nullptr) {
            Node* node = new (ptr) Node(forward<ARGS>(args)...);
            return node;
        }
        return nullptr;
    }

    inline void RemoveNode(Node* ptr) { m_allocator.free(ptr); }

   private:
    SceneGraph();

    SceneGraph(const SceneGraph&) = delete;
    SceneGraph& operator=(const SceneGraph&) = delete;
    SceneGraph(SceneGraph&&) = delete;
    SceneGraph& operator=(SceneGraph&&) = delete;

    PoolAllocator m_allocator;
    Node* m_world;
}