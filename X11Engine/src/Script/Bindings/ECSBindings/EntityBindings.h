#ifndef ENTTITY_BINDINGS_H
#define ENTTITY_BINDINGS_H

#include <lua.hpp>

#include "EntityId.h"
#include "lua.h"

class World;

namespace Engine::Script::Binding::ECS {

EntityId getEntityId(lua_State* state, int stack_index = 1);

void initEntityBindings(lua_State* state);

};  // namespace Engine::Script::Binding::ECS

#endif  // ENTTITY_BINDINGS_H