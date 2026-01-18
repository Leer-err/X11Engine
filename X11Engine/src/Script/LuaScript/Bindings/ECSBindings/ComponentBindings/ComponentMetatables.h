#pragma once

struct lua_State;

namespace Engine::Script::Binding::ECS {

constexpr char TRANSFORM_METATABLE[] = "Transform";
constexpr char STATIC_MESH_METATABLE[] = "StaticMesh";

void createComponentTable(lua_State* state);

void initComponentBindings(lua_State* state);

};  // namespace Engine::Script::Binding::ECS