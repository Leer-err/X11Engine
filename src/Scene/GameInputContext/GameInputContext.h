#pragma once

#include "InputContext.h"

namespace Input {

enum class GameActions {

};

enum class GameAxes {
    LookPitch,
    LookYaw,
    MoveForwardBackward,
    MoveLeftRight,
    MoveUpDown
};

using GameInputContext = InputContext<GameActions, GameAxes>;

}  // namespace Input