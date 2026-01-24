#pragma once

struct lua_State;

class Texture;

namespace Engine::Script::Binding {

Texture getTexture(lua_State* state, int index = 1);

void initTextureBindings(lua_State* state);

};  // namespace Engine::Script::Binding