#ifndef TRANSFORM_BINDINGS_H
#define TRANSFORM_BINDINGS_H

#include <lua.hpp>

struct ComponentId;

class World;
class Transform;

namespace Engine::Script::Binding::ECS {

void pushTransform(lua_State* state, ComponentId transform);

void initTransformBindings(lua_State* state);

};  // namespace Engine::Script::Binding::ECS

#endif  // TRANSFORM_BINDINGS_H