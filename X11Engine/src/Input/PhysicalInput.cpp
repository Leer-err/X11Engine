#include "PhysicalInput.h"

PhysicalInput::PhysicalInput()
    : current_axis_states(),
      current_key_states(),
      new_key_states(),
      new_axis_states(),
      mouse_mode(MouseMode::Relative) {
    for (int i = 0; i < current_key_states.size(); i++)
        current_key_states[i] = ButtonState::None;
    for (int i = 0; i < current_axis_states.size(); i++)
        current_axis_states[i] = 0;

    for (int i = 0; i < new_key_states.size(); i++)
        new_key_states[i] = ButtonState::None;
    for (int i = 0; i < new_axis_states.size(); i++) new_axis_states[i] = 0;
}

void PhysicalInput::saveState() {
    current_axis_states = new_axis_states;
    current_key_states = new_key_states;

    for (auto& state : new_key_states) {
        if (state == ButtonState::Pressed) state = ButtonState::Held;
        if (state == ButtonState::Released) state = ButtonState::None;
    }

    for (auto& axis : new_axis_states) {
        axis = 0.0f;
    }
}

void PhysicalInput::buttonPressed(Button key) {
    if (key == Button::INVALID_BUTTON) return;

    auto button_index = static_cast<std::underlying_type_t<Button>>(key);

    auto old_state = current_key_states[button_index];
    auto& new_state = new_key_states[button_index];
    switch (old_state) {
        case ButtonState::Held:
            break;
        case ButtonState::None:
            new_state = ButtonState::Pressed;
            break;
        case ButtonState::Pressed:
            break;
        case ButtonState::Released:
            new_state = ButtonState::Pressed;
            break;
    }
}

void PhysicalInput::buttonReleased(Button key) {
    if (key == Button::INVALID_BUTTON) return;

    auto button_index = static_cast<std::underlying_type_t<Button>>(key);

    auto old_state = current_key_states[button_index];
    auto& new_state = new_key_states[button_index];
    switch (old_state) {
        case ButtonState::Held:
            new_state = ButtonState::Released;
            break;
        case ButtonState::None:
            break;
        case ButtonState::Pressed:
            new_state = ButtonState::Released;
            break;
        case ButtonState::Released:
            break;
    }
}

void PhysicalInput::axisUpdated(Axis axis, float value) {
    if (axis == Axis::INVALID_AXIS) return;

    auto axis_index = static_cast<std::underlying_type_t<Axis>>(axis);

    new_axis_states[axis_index] = value;
}

ButtonState PhysicalInput::getButtonState(Button key) const {
    auto button_index = static_cast<std::underlying_type_t<Button>>(key);

    return current_key_states[button_index];
}

float PhysicalInput::getAxisState(Axis axis) const {
    auto axis_index = static_cast<std::underlying_type_t<Axis>>(axis);

    return current_axis_states[axis_index];
}

void PhysicalInput::setMouseMode(MouseMode mode) { mouse_mode = mode; }

MouseMode PhysicalInput::getMouseMode() const { return mouse_mode; }
