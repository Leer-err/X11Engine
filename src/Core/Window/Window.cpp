#include "Window.h"

#include <SDL3/SDL.h>

#include "AppConfig.h"
#include "LoggerFactory.h"
#include "PhysicalInput.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include "WindowConfig.h"
#include "backends/imgui_impl_sdl3.h"

using Input::Axis;
using Input::Button;
using Input::MouseMode;
using Input::PhysicalInput;

constexpr Button BUTTON_MAPPING_TABLE[] = {
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::KEYBOARD_A,      Button::KEYBOARD_B,
    Button::KEYBOARD_C,      Button::KEYBOARD_D,      Button::KEYBOARD_E,
    Button::KEYBOARD_F,      Button::KEYBOARD_G,      Button::KEYBOARD_H,
    Button::KEYBOARD_I,      Button::KEYBOARD_J,      Button::KEYBOARD_K,
    Button::KEYBOARD_L,      Button::KEYBOARD_M,      Button::KEYBOARD_N,
    Button::KEYBOARD_O,      Button::KEYBOARD_P,      Button::KEYBOARD_Q,
    Button::KEYBOARD_R,      Button::KEYBOARD_S,      Button::KEYBOARD_T,
    Button::KEYBOARD_U,      Button::KEYBOARD_V,      Button::KEYBOARD_W,
    Button::KEYBOARD_X,      Button::KEYBOARD_Y,      Button::KEYBOARD_Z,
    Button::KEYBOARD_1,      Button::KEYBOARD_2,      Button::KEYBOARD_3,
    Button::KEYBOARD_4,      Button::KEYBOARD_5,      Button::KEYBOARD_6,
    Button::KEYBOARD_7,      Button::KEYBOARD_8,      Button::KEYBOARD_9,
    Button::KEYBOARD_0,      Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::KEYBOARD_LCTRL,
    Button::KEYBOARD_LSHIFT, Button::KEYBOARD_LALT,   Button::INVALID_BUTTON,
    Button::KEYBOARD_RCTRL,  Button::KEYBOARD_RSHIFT, Button::KEYBOARD_RALT,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON,  Button::INVALID_BUTTON,
    Button::INVALID_BUTTON,  Button::INVALID_BUTTON};

Window::Window() : logger(LoggerFactory::getLogger("Window")) {}

Window::~Window() {
    SDL_DestroyWindow(handle);
    SDL_Quit();
}

void Window::init() {
    SDL_Init(SDL_INIT_VIDEO);

    auto config = Config::App::get().getWindowConfig();

    width = config.width;
    height = config.height;

    SDL_WindowFlags flags = SDL_WINDOW_VULKAN;

    switch (config.state) {
        case Config::Window::State::Windowed:
            break;
        case Config::Window::State::Fullscreen:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        case Config::Window::State::Borderless:
            flags |= SDL_WINDOW_BORDERLESS;
            break;
    }

    handle = SDL_CreateWindow("SDL3 Project", width, height, flags);

    // SDL_SetWindowRelativeMouseMode(handle, true);
}

bool Window::processMessages() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type) {
            case SDL_EVENT_QUIT:
                return false;
            case SDL_EVENT_KEY_DOWN:
                processKeyDown(&event);
                break;
            case SDL_EVENT_KEY_UP:
                processKeyUp(&event);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                processMouseButtonDown(&event);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                processMouseButtonUp(&event);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                processMouseMove(&event);
                break;
            default:
                break;
        }
    }

    return true;
}

void Window::processKeyDown(const SDL_Event* event) {
    auto scancode = event->key.scancode;

    if (scancode == SDL_SCANCODE_LALT) {
        MouseMode mode = PhysicalInput::get().getMouseMode();
        if (mode == MouseMode::Absolute) {
            PhysicalInput::get().setMouseMode(MouseMode::Relative);
            SDL_SetWindowRelativeMouseMode(handle, true);

            logger.debug("Switched mouse to relative mode");
        } else {
            PhysicalInput::get().setMouseMode(MouseMode::Absolute);
            SDL_SetWindowRelativeMouseMode(handle, false);

            logger.debug("Switched mouse to absolute mode");
        }
        return;
    }

    if (scancode >= SDL_SCANCODE_MODE) return;

    auto button = BUTTON_MAPPING_TABLE[scancode];
    PhysicalInput::get().buttonPressed(button);

    logger.debug("{} was pressed", SDL_GetKeyName(event->key.key));
}

void Window::processKeyUp(const SDL_Event* event) {
    auto scancode = event->key.scancode;

    if (scancode >= SDL_SCANCODE_MODE) return;

    auto button = BUTTON_MAPPING_TABLE[scancode];
    PhysicalInput::get().buttonReleased(button);

    logger.debug("{} was released", SDL_GetKeyName(event->key.key));
}

void Window::processMouseButtonDown(const SDL_Event* event) {
    Button button = Button::INVALID_BUTTON;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            button = Button::MOUSE_LB;
            break;
        case SDL_BUTTON_MIDDLE:
            button = Button::MOUSE_MB;
            break;
        case SDL_BUTTON_RIGHT:
            button = Button::MOUSE_RB;
            break;
        default:
            break;
    }

    PhysicalInput::get().buttonPressed(button);
}

void Window::processMouseButtonUp(const SDL_Event* event) {
    Button button = Button::INVALID_BUTTON;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            button = Button::MOUSE_LB;
            break;
        case SDL_BUTTON_MIDDLE:
            button = Button::MOUSE_MB;
            break;
        case SDL_BUTTON_RIGHT:
            button = Button::MOUSE_RB;
            break;
        default:
            break;
    }

    PhysicalInput::get().buttonReleased(button);
}

void Window::processMouseMove(const SDL_Event* event) {
    logger.debug("Mouse x: {}, y: {}", event->motion.x, event->motion.y);
    logger.debug("Mouse relative x: {}, y: {}", event->motion.xrel,
                 event->motion.yrel);

    float screen_space_x = event->motion.xrel / width;
    float screen_space_y = event->motion.yrel / height;

    PhysicalInput::get().axisUpdated(Axis::MOUSE_X, screen_space_x);
    PhysicalInput::get().axisUpdated(Axis::MOUSE_Y, screen_space_y);
}

SDL_Window* Window::getHandle() const { return handle; }