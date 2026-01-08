#ifndef ECS_BINDINGS_H
#define ECS_BINDINGS_H

#include "lua.hpp"

namespace Engine::Script::Binding::ECS {

void initBindings(lua_State* state);

};  // namespace Engine::Script::Binding::ECS

#endif  // ECS_BINDINGS_H