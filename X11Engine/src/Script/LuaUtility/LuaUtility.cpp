#include "LuaUtility.h"

#include "lua.hpp"

namespace Engine::Script::Utility {

void setGlobalName(lua_State* state, const std::string& name) {
    lua_setglobal(state, name.c_str());
}

void setMemberName(lua_State* state, const std::string& name, int table_index) {
    lua_setfield(state, table_index, name.c_str());
}

void getGlobalByName(lua_State* state, const std::string& name) {
    lua_getglobal(state, name.c_str());
}

void getMemberByName(lua_State* state, const std::string& name,
                     int table_index) {
    lua_getfield(state, -1, name.c_str());
}

void pushNil(lua_State* state) { lua_pushnil(state); }

};  // namespace Engine::Script::Utility