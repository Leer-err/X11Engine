#ifndef INPUT_H
#define INPUT_H

#include <cstddef>
#include <type_traits>

enum class ActionState { ON, OFF, ACTIVATED, DEACTIVATED };

enum class Action {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    ATTACK,
    TOGGLE_DEV_OVERLAY
};

constexpr auto ACTION_COUNT =
    static_cast<std::underlying_type_t<Action>>(Action::TOGGLE_DEV_OVERLAY) + 1;

typedef size_t ActionId;

struct InputState {
    ActionState actionStates[ACTION_COUNT];
};

class Input {
   public:
    static Input& get() {
        static Input instance = {};
        return instance;
    }

    InputState getState();
    void saveState();

    void activateAction(Action action);
    void deactivateAction(Action action);

   private:
    InputState previous_state;
    InputState current_state;

    Input();
};

#endif  // INPUT_H