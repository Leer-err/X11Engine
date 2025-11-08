#include <lua.hpp>

class World;

namespace Engine::Script::Binding {

void initBindings(lua_State* state, World* world);

};  // namespace Engine::Script::Binding