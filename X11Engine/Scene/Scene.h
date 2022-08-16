#pragma once

#include <stdint.h>

#include <vector>

#include "Logger/Logger.h"
#include "Memory/Memory.h"
#include "TaskManager/TaskManager.h"
#include "Types/Matrix.h"
#include "Types/Quaternion.h"
#include "Types/Vector3.h"
#include "Window.h"

using std::vector;

using Memory::PoolAllocator;

constexpr uint32_t MAX_NODE_COUNT = 0x400;

class Scene;

struct Transform {
    Transform(vector3 positions, vector3 scale, quaternion rotation)
        : position(positions), scale(scale), rotation(rotation) {}

    void CalcWorldMatrix(const matrix& parentMatrix) {
        matrix localMatrix = ScalingMatrix(scale) * RotationMatrix(rotation) *
                             TranslationMatrix(position);
        worldMatrix = parentMatrix * localMatrix;
    }

    vector3 position;
    vector3 scale;
    quaternion rotation;

    matrix worldMatrix;
};

class Node {
   public:
    Node(vector3 pos = {0.f, 0.f, 0.f}, vector3 scale = {1.f, 1.f, 1.f},
         quaternion rotation = {})
        : m_transform(pos, scale, rotation),
          m_parent(nullptr),
          m_isDirty(false) {}

    inline vector3 GetLocalPosition() const { return m_transform.position; }
    inline vector3 GetGlobalPosition() const {
        return m_transform.position * m_parent->m_transform.worldMatrix;
    }
    inline quaternion GetRotation() const { return m_transform.rotation; }
    inline vector3 GetScale() const { return m_transform.scale; }
    inline matrix GetWorldMatrix() const { return m_transform.worldMatrix; }

    inline void SetLocalPosition(const vector3& position) {
        m_transform.position = position;
        UpdateWorldMatrix();
    }
    inline void SetGlobalPosition(const vector3& position) {
        m_transform.position =
            position * m_parent->m_transform.worldMatrix.Inverse();
        UpdateWorldMatrix();
    }
    void SetRotation(const quaternion& rotation) {
        m_transform.rotation = rotation;
        UpdateWorldMatrix();
    }
    void SetScale(const vector3& scale) {
        m_transform.scale = scale;
        UpdateWorldMatrix();
    }

    template <typename... ARGS>
    Node* AddChild(ARGS&&... args) {
        Node* node = Scene::get().CreateNode(forward<ARGS>(args)...);
        if (node != nullptr) {
            node->m_parent = this;

            node->UpdateWorldMatrix();

            m_children.push_back(node);
        }

        return node;
    }

    void RemoveChild(Node* ptr) {
        auto& child = find(m_children.begin(), m_children.end(), ptr);

        if (child != m_children.end()) {
            m_isDirty = true;
        }
    }

    void UpdateWorldMatrix() {
        if (m_parent != nullptr) {
            m_transform.CalcWorldMatrix(m_parent->m_transform.worldMatrix);
        } else {
            m_transform.worldMatrix = IdentityMatrix();
        }

        for (auto& child : m_children) {
            TaskManager::get()->submit(&Node::UpdateWorldMatrix, *child);
        }
    }

    inline bool IsDirty() const { return m_isDirty; }

    inline vector<Node*> GetChildren() const { return m_children; }

   private:
    vector<Node*> m_children;
    Node* m_parent;

    bool m_isDirty;

    Transform m_transform;
};

class Scene {
    friend class Node;

   public:
    inline static Scene& get() {
        static Scene instance;
        return instance;
    }

    inline Node* GetWorldNode() { return m_world; }

    void ClearDirtyNodes(Node* start = nullptr) {
        if (start == nullptr) start = m_world;
        for (auto& child : start->GetChildren()) {
            if (child->IsDirty()) {
                ClearDirtyNodes(child);
            }
        }
    }

   protected:
    template <typename... ARGS>
    Node* CreateNode(ARGS&&... args) {
        void* ptr = m_allocator->allocate(sizeof(Node), _alignof(Node));
        if (ptr != nullptr) {
            Node* node = new (ptr) Node(forward<ARGS>(args)...);
            return node;
        }
        return nullptr;
    }

    inline void RemoveNode(Node* ptr) { m_allocator->free(ptr); }

   private:
    Scene() {
        void* ptr = Memory::GlobalAllocator::get()->allocate(
            MAX_NODE_COUNT * sizeof(Node), _alignof(Node));
        if (ptr == nullptr) {
            Logger::get()->Error(L"Cannot allocate memory for scene graph");
            Window::get()->Terminate();
        }
        m_allocator = new PoolAllocator(sizeof(Node), _alignof(Node),
                                        MAX_NODE_COUNT * sizeof(Node), ptr);
        m_world = CreateNode();
        m_world->UpdateWorldMatrix();
    }
    ~Scene() { delete (m_allocator); }

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    void ClearAllNodes(Node* node) {
        for (auto& child : node->GetChildren()) {
            ClearDirtyNodes(child);
        }
    }

    PoolAllocator* m_allocator;
    Node* m_world;
};