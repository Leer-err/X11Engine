#ifndef LUA_UTILITY_H
#define LUA_UTILITY_H

#include <lua.hpp>
#include <string>
#include <string_view>
#include <type_traits>

namespace Engine::Script::Utility {

void setGlobalName(lua_State* state, const std::string& name);

void setMemberName(lua_State* state, const std::string& name,
                   int table_index = -2);

void getGlobalByName(lua_State* state, const std::string& name);

void getMemberByName(lua_State* state, const std::string& name,
                     int table_index = -1);

void setMetatable(lua_State* state, const std::string& name,
                  int table_index = -1);

void pushNil(lua_State* state);

template <typename T>
void pushArgument(lua_State* state, T&& argument) {
    using ArgumentType = std::remove_cvref_t<T>;

    if constexpr (std::is_same_v<ArgumentType, bool>) {
        lua_pushboolean(state, argument);
        return;
    } else if constexpr (std::is_floating_point_v<ArgumentType>) {
        lua_pushnumber(state, argument);
        return;
    } else if constexpr (std::is_convertible_v<ArgumentType,
                                               std::string_view>) {
        auto view = std::string_view(argument);
        lua_pushlstring(state, view.data(), view.size());
        return;
    } else if constexpr (std::is_convertible_v<ArgumentType, long long>) {
        auto value = (long long)argument;
        lua_pushinteger(state, value);
        return;
    } else if constexpr (std::is_pointer_v<ArgumentType>) {
        lua_pushlightuserdata(state, argument);
        return;
    } else if constexpr (std::is_convertible_v<ArgumentType,
                                               int (*)(lua_State*)>) {
        auto function = (int (*)(lua_State*))(argument);
        lua_pushcfunction(state, function);
        return;
    } else {
        static_assert(false, "Type is not supported");
    }
}

template <typename T>
void setGlobalVariable(lua_State* state, const std::string& name,
                       T&& argument) {
    pushArgument(state, std::forward<T>(argument));
    setGlobalName(state, name);
}

template <typename T>
void setMemberVariable(lua_State* state, const std::string& name, T&& argument,
                       int table_index = -1) {
    pushArgument(state, std::forward<T>(argument));
    setMemberName(state, name, table_index - 1);
}

template <typename... ARGS>
void pushFunction(lua_State* state, int (*function)(lua_State*),
                  ARGS&&... args) {
    static_assert((std::is_pointer_v<std::decay_t<ARGS>> && ...),
                  "All args should be pointers");

    (pushArgument(state, std::forward<ARGS>(args)), ...);

    lua_pushcclosure(state, function, sizeof...(ARGS));
}

template <typename... ARGS>
void setGlobalFunction(lua_State* state, const std::string& name,
                       int (*function)(lua_State*), ARGS&&... args) {
    pushFunction(state, function, std::forward<ARGS>(args)...);
    setGlobalName(state, name);
}

template <typename... ARGS>
void setMemberFunction(lua_State* state, const std::string& name,
                       int (*function)(lua_State*), int table_index = -1,
                       ARGS&&... args) {
    pushFunction(state, function, std::forward<ARGS>(args)...);
    setMemberName(state, name, table_index - 1);
}

template <typename T>
T getArgument(lua_State* state, int index = -1) {
    using ArgumentType = std::remove_cvref_t<T>;

    if constexpr (std::is_same_v<ArgumentType, bool>) {
        return lua_toboolean(state, index);
    } else if constexpr (std::is_floating_point_v<ArgumentType>) {
        return lua_tonumber(state, index);
    } else if constexpr (std::is_convertible_v<std::string, ArgumentType>) {
        return lua_tostring(state, index);
    } else if constexpr (std::is_convertible_v<long long, ArgumentType>) {
        return lua_tointeger(state, index);
    } else if constexpr (std::is_convertible_v<ArgumentType,
                                               int (*)(lua_State*)>) {
        return lua_tocfunction(state, index);
    } else {
        static_assert(false, "Type is not supported");
    }
}

template <typename T>
T getGlobalVariable(lua_State* state, const std::string& name) {
    getGlobalByName(state, name);
    getArgument<T>(state);
}

template <typename T>
T getMemberVariable(lua_State* state, const std::string& name,
                    int table_index = -1) {
    getMemberByName(state, name, table_index);
    getArgument<T>(state);
}

};  // namespace Engine::Script::Utility

#endif  // LUA_UTILITY_H