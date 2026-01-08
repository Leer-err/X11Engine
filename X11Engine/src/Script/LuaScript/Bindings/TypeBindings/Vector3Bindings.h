#ifndef VECTOR_3_BINDINGS_H
#define VECTOR_3_BINDINGS_H

#include <lua.hpp>

#include "Vector3.h"

namespace Engine::Script::Binding::Types {

void pushVector3(lua_State* state, const Vector3& data);

Vector3 toVector3(lua_State* state, int index);

};  // namespace Engine::Script::Binding::Types

#endif  // VECTOR_3_BINDINGS_H