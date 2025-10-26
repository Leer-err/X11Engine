#include "EventListener.h"

namespace Engine::Event {

EventListener::EventListener(std::function<void(const Event* event)> callback)
    : callback(callback) {}

void EventListener::execute(const Event* event) { callback(event); }

};  // namespace Engine::Event