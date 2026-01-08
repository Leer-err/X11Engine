#ifndef CAMERA_BINDINGS_H
#define CAMERA_BINDINGS_H

#include <lua.hpp>

namespace Engine::Script::Binding::Camera {

void initBindings(lua_State* state);

};  // namespace Engine::Script::Binding::Camera

#endif  // CAMERA_BINDINGS_H