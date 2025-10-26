#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <vcruntime_typeinfo.h>

#include <memory>
#include <mutex>
#include <vector>

#include "Event.h"
#include "EventListener.h"
#include "EventSubscription.h"
#include "TypeId.h"
#include "TypeIdHelper.h"

namespace Engine::Event {

template <typename EventType>
using EventCallback = std::function<void(const EventType&)>;

class EventBus {
    struct QueuedEvent {
        std::shared_ptr<Event> event;
        TypeId event_type;
    };

   public:
    static EventBus& get() {
        static EventBus instance;
        return instance;
    }

    void flush();

    void clear();

    template <typename EventType>
    EventSubscription subscribe(EventCallback<EventType> callback) {
        auto callback_translated = [callback](const Event* event) {
            callback(*reinterpret_cast<const EventType*>(event));
        };

        auto listener = std::make_shared<EventListener>(callback_translated);

        std::lock_guard<std::mutex> lock(mutex);

        TypeId event_type = TypeIdHelper::getTypeId<EventType>();
        auto& listener_list = listeners[event_type];
        listener_list.push_back(listener);

        return EventSubscription([=]() { unsubscribe(event_type, listener); });
    }

    template <typename EventType>
    void publish(const EventType& event) {
        std::lock_guard<std::mutex> lock(mutex);

        event_queue.emplace_back(event);
    }

    template <typename EventType>
    void publish(EventType&& event) {
        std::lock_guard<std::mutex> lock(mutex);

        event_queue.emplace_back(
            QueuedEvent{std::make_shared<EventType>(event),
                        TypeIdHelper::getTypeId<EventType>()});
    }

   private:
    using ListenerList = std::vector<std::shared_ptr<EventListener>>;

    void process(const QueuedEvent& event);
    void unsubscribe(TypeId type_id,
                     const std::shared_ptr<EventListener>& listener);

    std::unordered_map<TypeId, ListenerList> listeners;

    std::vector<QueuedEvent> event_queue;

    std::mutex mutex;

    EventBus() = default;
    ~EventBus() = default;

    EventBus(EventBus&) = delete;
    EventBus& operator=(EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    EventBus& operator=(EventBus&&) = delete;
};

};  // namespace Engine::Event

#endif  // EVENT_BUS_H

struct ScriptEventType {};

struct ScriptEvent {
    ScriptEventType type;
};