#ifndef ENGINE_BINDINGS_H
#define ENGINE_BINDINGS_H

#include <lua.hpp>

namespace Engine::Script::Binding::Engine {

void initEngineBindings(lua_State* state);

};  // namespace Engine::Script::Binding::Engine

#endif  // ENGINE_BINDINGS_H