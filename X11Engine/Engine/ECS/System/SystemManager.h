#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "Framework/Memory/GlobalAllocator.h"
#include "Framework/Memory/Memory.h"
#include "System.h"

using std::forward;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

namespace ECS {
class SystemManager {
   public:
    inline static SystemManager* get() {
        static SystemManager instance;
        return &instance;
    }

    inline void Destroy() {
        for (auto system : m_workOrder) {
            delete (system);
        }
    }

    template <class T, class... Args>
    T* AddSystem(Args&&... args) {
        void* ptr = m_allocator->allocate(sizeof(T), alignof(T));

        ISystem* system = new (ptr) T(forward<Args>(args)...);

        assert(m_systems.find(T::TYPE_ID) == m_systems.end());
        m_systems.emplace(T::TYPE_ID, system);

        if (m_workOrder.size() == 0) {
            m_workOrder.push_back(system);
            return (T*)system;
        }
        for (int i = 0; i < m_workOrder.size(); i++) {
            if (m_workOrder[i]->m_priority <= system->m_priority) {
                m_workOrder.insert(m_workOrder.begin() + i, system);
                return (T*)system;
            }
        }

        m_workOrder.push_back(system);
        return (T*)system;
    }

    template <class T>
    inline T* GetSystem() const {
        auto system = m_systems.find(T::TYPE_ID);
        assert(system != m_systems.end() && "There is no system with this id");

        return (T*)system->second;
    }

    template <class T>
    void SetSystemUpdateInterval(uint32_t dt) {
        GetSystem<T>->m_updateInterval = dt;
    }

    template <class T>
    inline void SetSystemPriority(uint16_t priority) {
        GetSystem<T>->m_priority = priority;
    }

    inline void PreUpdate() {
        for (auto system : m_workOrder) {
            system->PreUpdate();
        }
    }

    inline void Update(float delta) {
        for (auto system : m_workOrder) {
            system->Update(delta);
        }
    }

    inline void PostUpdate() {
        for (auto system : m_workOrder) {
            system->PostUpdate();
        }
    }

   protected:
    SystemManager() {
        m_allocator =
            shared_ptr<Memory::LinearAllocator>(new Memory::LinearAllocator(
                Memory::GlobalAllocator::get()->allocate(8192, 0x40), 8192));
    }

   private:
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;

    shared_ptr<Memory::LinearAllocator> m_allocator;
    unordered_map<TypeId, ISystem*> m_systems;
    vector<ISystem*> m_workOrder;
};
}  // namespace ECS