#include "EventBus.h"

#include <memory>
#include <mutex>

#include "Event.h"

namespace Engine::Event {

void EventBus::flush() {
    mutex.lock();
    std::vector<QueuedEvent> events_to_process = std::move(event_queue);
    mutex.unlock();

    for (const auto& event : events_to_process) {
        process(event);
    }
}

void EventBus::clear() {
    std::lock_guard<std::mutex> lock(mutex);

    listeners.clear();
    event_queue.clear();
}

void EventBus::process(const QueuedEvent& event) {
    TypeId event_type = event.event_type;

    mutex.lock();
    auto listeners_it = listeners.find(event_type);
    if (listeners_it == listeners.end()) {
        mutex.unlock();
        return;
    }

    ListenerList event_listeners = listeners_it->second;
    mutex.unlock();

    for (auto& listener : event_listeners) {
        listener->execute(event.event.get());
    }
}

void EventBus::unsubscribe(TypeId type_id,
                           const std::shared_ptr<EventListener>& listener) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = listeners.find(type_id);
    if (it == listeners.end()) return;

    auto& event_listeners = it->second;
    event_listeners.erase(
        std::find(event_listeners.begin(), event_listeners.end(), listener));

    if (event_listeners.empty()) {
        listeners.erase(it);
    }
}

};  // namespace Engine::Event