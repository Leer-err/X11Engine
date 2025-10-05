#ifndef GAME_INPUT_CONTEXT_H
#define GAME_INPUT_CONTEXT_H

#include "InputContext.h"

enum GameAction { TOGGLE_OVERLAY };
enum GameAxis { MOVE_FORWARD_BACKWARD, MOVE_LEFT_RIGHT, LOOK_X, LOOK_Y };

class GameInputContext : public InputContext<GameAction, GameAxis> {
   public:
    static GameInputContext& get() {
        static GameInputContext context = {};
        return context;
    }

   private:
    GameInputContext() = default;

    GameInputContext(const GameInputContext&) = default;
    GameInputContext& operator=(const GameInputContext&) = default;
    GameInputContext& operator=(GameInputContext&&) = default;
};

#endif  // GAME_INPUT_CONTEXT_H