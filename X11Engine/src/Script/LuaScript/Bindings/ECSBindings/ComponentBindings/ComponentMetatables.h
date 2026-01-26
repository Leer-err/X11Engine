#pragma once

#include "TypeId.h"

struct lua_State;
class Entity;

namespace Engine::Script::Binding::ECS {

constexpr char TRANSFORM_METATABLE[] = "Transform";
constexpr char STATIC_MESH_METATABLE[] = "StaticMesh";
constexpr char STATIC_MATERIAL_METATABLE[] = "StaticMaterial";

void addComponent(lua_State* state, Entity entity, TypeId component_type);

int getComponent(lua_State* state, Entity entity, TypeId component_type);

void createComponentTable(lua_State* state);

void initComponentBindings(lua_State* state);

};  // namespace Engine::Script::Binding::ECS