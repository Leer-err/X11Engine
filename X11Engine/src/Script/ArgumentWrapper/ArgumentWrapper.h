#ifndef ARGUMENT_WRAPPER_H
#define ARGUMENT_WRAPPER_H

#include <functional>
#include <string>
#include <string_view>
#include <type_traits>

#include "lua.h"
#include "lua.hpp"

template <typename T>
void pushArgument(lua_State* state, T&& argument) {
    using ArgumentType = std::remove_cvref_t<T>;

    if constexpr (std::is_same_v<ArgumentType, bool>) {
        lua_pushboolean(state, argument);
        return;
    } else if constexpr (std::is_integral_v<ArgumentType>) {
        lua_pushinteger(state, argument);
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
    } else if constexpr (std::is_convertible_v<ArgumentType,
                                               int (*)(lua_State*)>) {
        auto function = (int (*)(lua_State*))(argument);
        lua_pushcfunction(state, function);
        return;
    }

    throw std::string("Type is not supported");
}

#endif  // ARGUMENT_WRAPPER_H