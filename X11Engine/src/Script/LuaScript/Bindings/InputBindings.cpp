#include "InputBindings.h"

#include <tracy/Tracy.hpp>

#include "GameInputContext.h"
#include "LuaUtility.h"
#include "PhysicalInput.h"
#include "lua.h"

namespace Engine::Script::Binding::Input {

namespace Utility = Engine::Script::Utility;

void loadContextBindings(lua_State* state) {
    lua_newtable(state);
    Utility::setMemberVariable(state, "TOGGLE_OVERLAY", TOGGLE_OVERLAY);
    Utility::setGlobalName(state, "GameAction");

    lua_newtable(state);
    Utility::setMemberVariable(state, "MOVE_FORWARD_BACKWARD",
                               MOVE_FORWARD_BACKWARD);
    Utility::setMemberVariable(state, "MOVE_RIGHT_LEFT", MOVE_RIGHT_LEFT);
    Utility::setMemberVariable(state, "LOOK_X", LOOK_X);
    Utility::setMemberVariable(state, "LOOK_Y", LOOK_Y);
    Utility::setGlobalName(state, "GameAxis");
}

extern "C" int isPressed(lua_State* state) {
    ZoneScoped;
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Pressed;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int isReleased(lua_State* state) {
    ZoneScoped;
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Released;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int getAction(lua_State* state) {
    ZoneScoped;
    GameAction action = (GameAction)lua_tointeger(state, 1);

    auto button_state = GameInputContext::get().getButton(action);
    bool result = button_state == ButtonState::Held ||
                  button_state == ButtonState::Pressed;

    Utility::pushArgument(state, result);

    return 1;
}

extern "C" int getAxis(lua_State* state) {
    ZoneScoped;
    GameAxis action = (GameAxis)lua_tointeger(state, 1);

    auto axis_value = GameInputContext::get().getAxis(action);

    Utility::pushArgument(state, axis_value);

    return 1;
}

void initInputBindings(lua_State* state) {
    loadContextBindings(state);

    lua_newtable(state);
    Utility::setMemberFunction(state, "isPressed", isPressed);
    Utility::setMemberFunction(state, "isReleased", isReleased);
    Utility::setMemberFunction(state, "getAction", getAction);
    Utility::setMemberFunction(state, "getAxis", getAxis);
    Utility::setGlobalName(state, "GameInput");
}

};  // namespace Engine::Script::Binding::Input