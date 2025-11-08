#ifndef ENTTITY_BINDINGS_H
#define ENTTITY_BINDINGS_H

#include <lua.hpp>

#include "lua.h"

class World;

namespace Engine::Script::Binding::ECS {

void initEntityBindings(lua_State* state, World* world);

};  // namespace Engine::Script::Binding::ECS

#endif  // ENTTITY_BINDINGS_H