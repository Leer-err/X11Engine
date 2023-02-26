#include "Scene.h"

void Scene::Node::RemoveChild(Node* ptr) {
    auto& child = find(m_children.begin(), m_children.end(), ptr);

    if (child != m_children.end()) {
        Scene::get()->RemoveNode(*child);
    }
}

void Scene::Node::UpdateWorldMatrix() {
    if (m_parent != nullptr) {
        m_transform.CalcWorldMatrix(m_parent->m_transform.worldMatrix);
    } else {
        m_transform.worldMatrix = IdentityMatrix();
    }

    for (auto& child : m_children) {
        Scene::get()->AddUpdateTask(
            move(TaskManager::get()->submit(&Node::UpdateWorldMatrix, child)));
    }
}

void Scene::Node::SetModel(Model* model) { m_model = model; }

Scene::Scene() {
    void* ptr = Memory::GlobalAllocator::get()->allocate(
        MAX_NODE_COUNT * sizeof(Node), _alignof(Node));
    if (ptr == nullptr) {
        Logger::get()->Error(L"Cannot allocate memory for scene graph node");
        Window::get()->Terminate();
    }
    m_allocator = new PoolAllocator(sizeof(Node), _alignof(Node),
                                    MAX_NODE_COUNT * sizeof(Node), ptr);
    m_world = CreateNode();
    m_world->UpdateWorldMatrix();
}

void Scene::WaitForUpdate() {
    for (const auto& task : m_updateTasks) task.wait();
    m_updateTasks.clear();
}