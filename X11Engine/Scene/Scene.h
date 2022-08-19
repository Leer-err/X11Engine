#pragma once

#include <stdint.h>

#include <future>
#include <mutex>
#include <vector>

#include "Graphics/Model.h"
#include "Logger/Logger.h"
#include "Memory/Memory.h"
#include "TaskManager/TaskManager.h"
#include "Types/Matrix.h"
#include "Types/Quaternion.h"
#include "Types/Vector3.h"
#include "Window.h"

using std::future;
using std::lock_guard;
using std::move;
using std::mutex;
using std::vector;

using Memory::PoolAllocator;

constexpr uint32_t MAX_NODE_COUNT = 0x400;

class Scene {
   public:
    struct Transform {
        Transform(vector3 positions, vector3 scale, quaternion rotation)
            : position(positions), scale(scale), rotation(rotation) {}

        void CalcWorldMatrix(const matrix& parentMatrix);

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
              m_model(nullptr) {
            UpdateWorldMatrix();
        }
        ~Node() {
            for (auto& child : m_children) {
                Scene::get()->RemoveNode(child);
            }
        }

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
        inline void SetRotation(const quaternion& rotation) {
            m_transform.rotation = rotation;
            UpdateWorldMatrix();
        }
        inline void SetScale(const vector3& scale) {
            m_transform.scale = scale;
            UpdateWorldMatrix();
        }

        inline void SetModel(Model* model) { m_model = model; }

        template <typename... ARGS>
        Node* AddChild(ARGS&&... args) {
            Node* node = Scene::get()->CreateNode(forward<ARGS>(args)...);
            if (node != nullptr) {
                node->m_parent = this;

                node->UpdateWorldMatrix();

                m_children.push_back(node);
            }

            return node;
        }

        void RemoveChild(Node* ptr);

        void UpdateWorldMatrix();

        inline vector<Node*> GetChildren() const { return m_children; }
        inline Model* GetModel() const { return m_model; }

       private:
        vector<Node*> m_children;
        Node* m_parent;

        Model* m_model;

        Scene::Transform m_transform;
    };

    inline static Scene* get() {
        static Scene instance;
        return &instance;
    }

    inline Node* GetWorldNode() { return m_world; }

    void WaitForUpdate();

   protected:
    template <typename... ARGS>
    Node* CreateNode(ARGS&&... args) {
        m_allocatorMutex.lock();
        void* ptr = m_allocator->allocate(sizeof(Node), _alignof(Node));
        m_allocatorMutex.unlock();
        if (ptr != nullptr) {
            Node* node = new (ptr) Node(forward<ARGS>(args)...);
            return node;
        }
        return nullptr;
    }

    inline void RemoveNode(Node* ptr) {
        lock_guard lock(m_allocatorMutex);
        m_allocator->free(ptr);
    }

    void AddUpdateTask(future<void>&& task) {
        m_updateTasks.emplace_back(move(task));
    }

   private:
    Scene();
    ~Scene() { delete (m_allocator); }

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    PoolAllocator* m_allocator;
    Node* m_world;
    mutex m_allocatorMutex;

    vector<future<void>> m_updateTasks;
};