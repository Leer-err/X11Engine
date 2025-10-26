#include "EventSubscription.h"

#include <optional>

namespace Engine::Event {

EventSubscription::EventSubscription() : unsubscribe_callback(std::nullopt) {}

EventSubscription::EventSubscription(
    const std::function<void()>& unsubscribe_callback)
    : unsubscribe_callback(unsubscribe_callback) {}

EventSubscription::~EventSubscription() {
    if (unsubscribe_callback) {
        (*unsubscribe_callback)();
    }
}

EventSubscription::EventSubscription(EventSubscription&& other) {
    std::swap(unsubscribe_callback, other.unsubscribe_callback);
}

EventSubscription& EventSubscription::operator=(EventSubscription&& other) {
    std::swap(unsubscribe_callback, other.unsubscribe_callback);

    return *this;
}

};  // namespace Engine::Event