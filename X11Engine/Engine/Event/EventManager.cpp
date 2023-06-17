#include "EventManager.h"

#include <vector>

#include "Framework/TaskManager/TaskManager.h"

using std::vector;

void EventManager::DispatchEvents() {
    vector<future<void>> tasks;
    m_eventMutex.lock();
    m_callbackMutex.lock();
    for (const Event* event : m_eventQueue) {
        auto callbacks = m_callbacks.find(event->GetEventType());
        if (callbacks != m_callbacks.end()) {
            for (auto callback : callbacks->second) {
                tasks.emplace_back(TaskManager::get()->submit(
                    &EventDelegate::invoke, callback, event));
            }
        }

        for (const auto& task : tasks) {
            task.wait();
        }
    }
    m_callbackMutex.unlock();

    m_eventQueue.clear();
    m_eventAllocator->clear();
    m_eventMutex.unlock();
}