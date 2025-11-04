#include "InputBindings.h"

#include "GameInputContext.h"
#include "LuaUtility.h"
#include "PhysicalInput.h"
#include "lua.h"

namespace Utility = Engine::Script::Utility;

namespace Engine::Script::Binding::Input {

void loadBindings(ScriptSandbox& sandbox) {
    sandbox.createTable("GameAction")
        .addProperty("TOGGLE_OVERLAY", TOGGLE_OVERLAY);

    sandbox.createTable("GameAxis")
        .addProperty("MOVE_FORWARD_BACKWARD", MOVE_FORWARD_BACKWARD)
        .addProperty("MOVE_LEFT_RIGHT", MOVE_LEFT_RIGHT)
        .addProperty("LOOK_X", LOOK_X)
        .addProperty("LOOK_Y", LOOK_Y);
}

extern "C" int isPressed(lua_State* state) {
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Pressed;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int isReleased(lua_State* state) {
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Released;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int getAction(lua_State* state) {
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Held ||
                  button_state == ButtonState::Pressed;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int getAxis(lua_State* state) {
    GameAxis action = (GameAxis)lua_tointeger(state, 1);

    auto axis_value = GameInputContext::get().getAxis(action);

    Utility::pushArgument(state, axis_value);

    return 1;
}

void initInputBindings(ScriptSandbox& sandbox) {
    loadBindings(sandbox);

    sandbox.createTable("GameInput")
        .addProperty("isPressed", isPressed)
        .addProperty("isReleased", isReleased)
        .addProperty("getAction", getAction)
        .addProperty("getAxis", getAxis);
}

};  // namespace Engine::Script::Binding::Input