#include "ObserverBuilder.h"

#include <functional>

#include "ObserverDispatcher.h"

ObserverBuilder::ObserverBuilder(TypeId component,
                                 ObserverDispatcher& dispatcher)
    : id(component), dispatcher(dispatcher) {}

ObserverBuilder& ObserverBuilder::on(Event event) {
    this->event = event;
    return *this;
}

ObserverBuilder& ObserverBuilder::call(std::function<void(Entity)> callback) {
    dispatcher.add(id, event, callback);
    return *this;
}