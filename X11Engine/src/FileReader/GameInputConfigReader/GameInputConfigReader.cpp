#include "GameInputConfigReader.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "GameInputContext.h"
#include "PhysicalInput.h"

using namespace nlohmann;

void GameInputConfigReader::read(const std::string& file_name,
                                 GameInputContext& context) {
    std::ifstream file(file_name);
    if (!file) return;

    json data = json::parse(file);

    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string logical_name = it.key();
        std::string physical_name = it.value().get<std::string>();

        if (logical_name == "move_forward")
            context.addBinding(GameAxis::MOVE_FORWARD_BACKWARD,
                               getPhysicalButton(physical_name), 1.0f);
        if (logical_name == "move_backward")
            context.addBinding(GameAxis::MOVE_FORWARD_BACKWARD,
                               getPhysicalButton(physical_name), -1.0f);
        if (logical_name == "move_left")
            context.addBinding(GameAxis::MOVE_LEFT_RIGHT,
                               getPhysicalButton(physical_name), -1.0f);
        if (logical_name == "move_right")
            context.addBinding(GameAxis::MOVE_LEFT_RIGHT,
                               getPhysicalButton(physical_name), 1.0f);
        if (logical_name == "toggle_overlay")
            context.addBinding(GameAction::TOGGLE_OVERLAY,
                               getPhysicalButton(physical_name));
        if (logical_name == "look_x")
            context.addBinding(GameAxis::LOOK_X,
                               getPhysicalAxis(physical_name));
        if (logical_name == "look_y")
            context.addBinding(GameAxis::LOOK_Y,
                               getPhysicalAxis(physical_name));
    }
}

Button GameInputConfigReader::getPhysicalButton(const std::string& name) const {
    if (name == "LMB") return MOUSE_LB;
    if (name == "MMB") return MOUSE_MB;
    if (name == "RMB") return MOUSE_RB;
    if (name == "0") return KEYBOARD_0;
    if (name == "1") return KEYBOARD_1;
    if (name == "2") return KEYBOARD_2;
    if (name == "3") return KEYBOARD_3;
    if (name == "4") return KEYBOARD_4;
    if (name == "5") return KEYBOARD_5;
    if (name == "6") return KEYBOARD_6;
    if (name == "7") return KEYBOARD_7;
    if (name == "8") return KEYBOARD_8;
    if (name == "9") return KEYBOARD_9;
    if (name == "A") return KEYBOARD_A;
    if (name == "B") return KEYBOARD_B;
    if (name == "C") return KEYBOARD_C;
    if (name == "D") return KEYBOARD_D;
    if (name == "E") return KEYBOARD_E;
    if (name == "F") return KEYBOARD_F;
    if (name == "G") return KEYBOARD_G;
    if (name == "H") return KEYBOARD_H;
    if (name == "I") return KEYBOARD_I;
    if (name == "J") return KEYBOARD_J;
    if (name == "K") return KEYBOARD_K;
    if (name == "L") return KEYBOARD_L;
    if (name == "M") return KEYBOARD_M;
    if (name == "N") return KEYBOARD_N;
    if (name == "O") return KEYBOARD_O;
    if (name == "P") return KEYBOARD_P;
    if (name == "Q") return KEYBOARD_Q;
    if (name == "R") return KEYBOARD_R;
    if (name == "S") return KEYBOARD_S;
    if (name == "T") return KEYBOARD_T;
    if (name == "U") return KEYBOARD_U;
    if (name == "V") return KEYBOARD_V;
    if (name == "W") return KEYBOARD_W;
    if (name == "X") return KEYBOARD_X;
    if (name == "Y") return KEYBOARD_Y;
    if (name == "Z") return KEYBOARD_Z;

    return INVALID_BUTTON;
}

Axis GameInputConfigReader::getPhysicalAxis(const std::string& name) const {
    if (name == "mouse_x") return MOUSE_X;
    if (name == "mouse_y") return MOUSE_Y;

    return INVALID_AXIS;
}
