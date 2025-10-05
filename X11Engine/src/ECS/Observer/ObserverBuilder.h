#ifndef OBSERVER_BUILDER_H
#define OBSERVER_BUILDER_H

#include <functional>

#include "Entity.h"
#include "ObserverDispatcher.h"
#include "TypeId.h"

class ObserverBuilder {
    friend class World;

   protected:
    ObserverBuilder(TypeId component, ObserverDispatcher& dispatcher);

   public:
    ObserverBuilder& on(Event event);
    ObserverBuilder& call(std::function<void(Entity)> callback);

   private:
    ObserverDispatcher& dispatcher;

    TypeId id;
    Event event;
};

#endif  // OBSERVER_BUILDER_H