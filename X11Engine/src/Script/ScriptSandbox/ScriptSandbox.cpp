#include "ScriptSandbox.h"

#include <memory>
#include <optional>

#include "Bindings.h"
#include "LoggerFactory.h"
#include "LuaUtility.h"
#include "World.h"
#include "lua.h"

using namespace Engine::Script;

ScriptSandbox::StateWrapper::StateWrapper() : state(luaL_newstate()) {}

ScriptSandbox::StateWrapper::~StateWrapper() { lua_close(state); }

ScriptSandbox::ScriptSandbox()
    : wrapper(std::make_shared<StateWrapper>()),
      logger(LoggerFactory::getLogger("ScriptSandbox")) {
    luaL_openlibs(wrapper->state);
}

ScriptSandbox::~ScriptSandbox() {}

void ScriptSandbox::initBindings(World& world) {
    Binding::initBindings(wrapper->state, &world);
}

std::optional<ScriptError> ScriptSandbox::runFile(const std::string& filename) {
    int result = luaL_loadfile(wrapper->state, filename.c_str());
    if (result != LUA_OK) return translateExecutionError(result);

    result = lua_pcall(wrapper->state, 0, LUA_MULTRET, 0);
    if (result != LUA_OK) {
        auto error_message = lua_tostring(wrapper->state, -1);
        logger.error("Script failed with : {}", error_message);
        lua_pop(wrapper->state, 1);

        return translateExecutionError(result);
    }

    return std::nullopt;
}

std::optional<ScriptError> ScriptSandbox::runString(const std::string& script) {
    int result = luaL_loadstring(wrapper->state, script.c_str());
    if (result != LUA_OK) return translateExecutionError(result);

    result = lua_pcall(wrapper->state, 0, LUA_MULTRET, 0);
    if (result != LUA_OK) {
        auto error_message = lua_tostring(wrapper->state, -1);
        logger.error("Script failed with : {}", error_message);
        lua_pop(wrapper->state, 1);

        return translateExecutionError(result);
    }

    return std::nullopt;
}

ScriptSandbox::StringResult ScriptSandbox::getString(
    const std::string& variable_name) {
    auto result = lua_getglobal(wrapper->state, variable_name.c_str());
    if (result != LUA_TSTRING) {
        lua_pop(wrapper->state, 1);
        return ValueError::WrongType;
    }

    auto value = lua_tostring(wrapper->state, -1);
    lua_pop(wrapper->state, 1);
    return std::string(value);
}

ScriptSandbox::NumberResult ScriptSandbox::getNumber(
    const std::string& variable_name) {
    auto result = lua_getglobal(wrapper->state, variable_name.c_str());
    if (result != LUA_TNUMBER) {
        lua_pop(wrapper->state, 1);
        return ValueError::WrongType;
    }

    auto value = lua_tonumber(wrapper->state, -1);
    lua_pop(wrapper->state, 1);
    return value;
}

ScriptError ScriptSandbox::translateExecutionError(int error) {
    if (error == LUA_ERRFILE) return ScriptError::FileNotFound;
    if (error == LUA_ERRSYNTAX) return ScriptError::SyntaxError;
    if (error == LUA_ERRMEM) return ScriptError::NoMemory;
    if (error == LUA_ERRRUN) return ScriptError::RuntimeError;

    throw;
}