#include "Input.h"

Input::Input() : previous_state(), current_state() {
    for (int i = 0; i < ACTION_COUNT; i++) {
        previous_state.actionStates[i] = ActionState::OFF;
    }
    for (int i = 0; i < ACTION_COUNT; i++) {
        current_state.actionStates[i] = ActionState::OFF;
    }
}

InputState Input::getState() { return previous_state; }

void Input::saveState() {
    previous_state = current_state;

    for (auto& state : current_state.actionStates) {
        if (state == ActionState::ACTIVATED) state = ActionState::ON;
        if (state == ActionState::DEACTIVATED) state = ActionState::OFF;
    }
}

void Input::activateAction(Action action) {
    auto action_index = static_cast<std::underlying_type_t<Action>>(action);

    auto old_state = previous_state.actionStates[action_index];
    auto& new_state = current_state.actionStates[action_index];
    switch (old_state) {
        case ActionState::ON:
            break;
        case ActionState::OFF:
            new_state = ActionState::ACTIVATED;
            break;
        case ActionState::ACTIVATED:
            new_state = ActionState::ON;
            break;
        case ActionState::DEACTIVATED:
            break;
    }
}

void Input::deactivateAction(Action action) {
    auto action_index = static_cast<std::underlying_type_t<Action>>(action);

    auto old_state = previous_state.actionStates[action_index];
    auto& new_state = current_state.actionStates[action_index];
    switch (old_state) {
        case ActionState::ON:
            new_state = ActionState::DEACTIVATED;
            break;
        case ActionState::OFF:
            break;
        case ActionState::ACTIVATED:
            new_state = ActionState::DEACTIVATED;
            break;
        case ActionState::DEACTIVATED:
            break;
    }
}
