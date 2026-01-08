#include "Vector3Bindings.h"

#include "LuaUtility.h"

namespace Engine::Script::Binding::Types {

void pushVector3(lua_State* state, const Vector3& data) {
    lua_createtable(state, 0, 3);

    Utility::setMemberVariable(state, "x", data.x);
    Utility::setMemberVariable(state, "y", data.y);
    Utility::setMemberVariable(state, "z", data.z);
    Utility::setMetatable(state, "Vector3");
}

Vector3 toVector3(lua_State* state, int index) {
    double x = Utility::getMemberVariable<double>(state, "x", index);
    double y = Utility::getMemberVariable<double>(state, "y", index);
    double z = Utility::getMemberVariable<double>(state, "z", index);

    return Vector3(x, y, z);
}

};  // namespace Engine::Script::Binding::Types