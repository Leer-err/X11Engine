#ifndef SCRIPT_SANDBOX_H
#define SCRIPT_SANDBOX_H

#include <lua.hpp>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include "LoggerFactory.h"
#include "LuaUtility.h"
#include "Result.h"
#include "lua.h"

namespace Engine::Script {

enum class ScriptError {
    SyntaxError,
    NoMemory,
    FileNotFound,
    NotCallable,
    RuntimeError
};
enum class ValueError { NotFound, WrongType };

class ScriptSandbox {
    struct StateWrapper {
        StateWrapper();
        ~StateWrapper();

        lua_State* state;
    };

   public:
    class TableBuilder {
        friend class ScriptSandbox;

       public:
        template <typename T>
        TableBuilder& addProperty(const std::string& name, T&& value) {
            Utility::setMemberVariable(state, name, value);

            return *this;
        }

        template <typename... ARGS>
        TableBuilder& addMethod(const std::string& name,
                                int (*function)(lua_State*), ARGS&&... args) {
            Utility::setMemberFunction(state, name, function, -1,
                                       std::forward<ARGS>(args)...);

            return *this;
        }

        ~TableBuilder();

       protected:
        TableBuilder(lua_State* state, const std::string& name);

       private:
        std::string name;

        lua_State* state;
    };

    ScriptSandbox();
    ~ScriptSandbox();

    std::optional<ScriptError> runFile(const std::string& filename);
    std::optional<ScriptError> runString(const std::string& script);

    template <typename... ARGS>
    std::optional<ScriptError> runFunction(const std::string& name,
                                           ARGS&&... args) {
        Utility::getGlobalByName(wrapper->state, name);

        (Utility::pushArgument(wrapper->state, std::forward<ARGS>(args)), ...);

        int result = lua_pcall(wrapper->state, sizeof...(args), 0, 0);

        if (result != LUA_OK) {
            auto error_message = lua_tostring(wrapper->state, -1);
            logger.error("Script failed with : {}", error_message);
            lua_pop(wrapper->state, 1);

            return translateExecutionError(result);
        }

        return std::nullopt;
    }

    template <typename... ARGS>
    void registerFunction(const std::string& name, int (*function)(lua_State*),
                          ARGS&&... args) {
        Utility::setGlobalFunction(wrapper->state, name, function,
                                   std::forward<ARGS>(args)...);
    }

    TableBuilder createTable(const std::string name);

    using StringResult = Result<std::string, ValueError>;
    StringResult getString(const std::string& variable_name);

    using NumberResult = Result<double, ValueError>;
    NumberResult getNumber(const std::string& variable_name);

   private:
    ScriptError translateExecutionError(int error);
    ScriptError translateValueError(int error);

    std::shared_ptr<StateWrapper> wrapper;

    Logger logger;
};
};  // namespace Engine::Script

#endif  // SCRIPT_SANDBOX_H