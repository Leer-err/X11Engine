#ifndef PHYSICAL_INPUT_H
#define PHYSICAL_INPUT_H

#include <array>

enum Button {
    MOUSE_LB,
    MOUSE_RB,
    MOUSE_MB,
    KEYBOARD_0,
    KEYBOARD_1,
    KEYBOARD_2,
    KEYBOARD_3,
    KEYBOARD_4,
    KEYBOARD_5,
    KEYBOARD_6,
    KEYBOARD_7,
    KEYBOARD_8,
    KEYBOARD_9,
    KEYBOARD_A,
    KEYBOARD_B,
    KEYBOARD_C,
    KEYBOARD_D,
    KEYBOARD_E,
    KEYBOARD_F,
    KEYBOARD_G,
    KEYBOARD_H,
    KEYBOARD_I,
    KEYBOARD_J,
    KEYBOARD_K,
    KEYBOARD_L,
    KEYBOARD_M,
    KEYBOARD_N,
    KEYBOARD_O,
    KEYBOARD_P,
    KEYBOARD_Q,
    KEYBOARD_R,
    KEYBOARD_S,
    KEYBOARD_T,
    KEYBOARD_U,
    KEYBOARD_V,
    KEYBOARD_W,
    KEYBOARD_X,
    KEYBOARD_Y,
    KEYBOARD_Z,
    INVALID_BUTTON,
};
constexpr auto KEY_COUNT =
    static_cast<std::underlying_type_t<Button>>(Button::KEYBOARD_Z) + 1;

enum Axis { MOUSE_X, MOUSE_Y, INVALID_AXIS };
constexpr auto AXES_COUNT =
    static_cast<std::underlying_type_t<Axis>>(Axis::MOUSE_Y) + 1;

enum class ButtonState { Pressed, Released, Held, None };

enum class MouseMode { Relative, Absolute };

class PhysicalInput {
   public:
    static PhysicalInput& get() {
        static PhysicalInput instance = {};
        return instance;
    }

    void saveState();

    void buttonPressed(Button key);
    void buttonReleased(Button key);

    void axisUpdated(Axis axis, float value);

    void setMouseMode(MouseMode mode);
    MouseMode getMouseMode() const;

    ButtonState getButtonState(Button key) const;
    float getAxisState(Axis axis) const;

   private:
    PhysicalInput();

    MouseMode mouse_mode;

    std::array<ButtonState, KEY_COUNT> current_key_states;
    std::array<float, AXES_COUNT> current_axis_states;

    std::array<ButtonState, KEY_COUNT> new_key_states;
    std::array<float, AXES_COUNT> new_axis_states;
};

#endif  // PHYSICAL_INPUT_H