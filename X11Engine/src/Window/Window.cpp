#include "Window.h"

#include <Windows.h>
#include <Windowsx.h>
#include <backends/imgui_impl_win32.h>
#include <winuser.h>

#include "PhysicalInput.h"
#include "WindowConfig.h"

constexpr auto WINDOW_CLASS_NAME = "WindowClass";

constexpr Button button_mapping[] = {
    Button::INVALID_BUTTON, Button::MOUSE_LB,       Button::MOUSE_RB,
    Button::INVALID_BUTTON, Button::MOUSE_MB,       Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::KEYBOARD_0,     Button::KEYBOARD_1,     Button::KEYBOARD_2,
    Button::KEYBOARD_3,     Button::KEYBOARD_4,     Button::KEYBOARD_5,
    Button::KEYBOARD_6,     Button::KEYBOARD_7,     Button::KEYBOARD_8,
    Button::KEYBOARD_9,     Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::KEYBOARD_A,
    Button::KEYBOARD_B,     Button::KEYBOARD_C,     Button::KEYBOARD_D,
    Button::KEYBOARD_E,     Button::KEYBOARD_F,     Button::KEYBOARD_G,
    Button::KEYBOARD_H,     Button::KEYBOARD_I,     Button::KEYBOARD_J,
    Button::KEYBOARD_K,     Button::KEYBOARD_L,     Button::KEYBOARD_M,
    Button::KEYBOARD_N,     Button::KEYBOARD_O,     Button::KEYBOARD_P,
    Button::KEYBOARD_Q,     Button::KEYBOARD_R,     Button::KEYBOARD_S,
    Button::KEYBOARD_T,     Button::KEYBOARD_U,     Button::KEYBOARD_V,
    Button::KEYBOARD_W,     Button::KEYBOARD_X,     Button::KEYBOARD_Y,
    Button::KEYBOARD_Z,     Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
    Button::INVALID_BUTTON, Button::INVALID_BUTTON, Button::INVALID_BUTTON,
};

Window::Window() {}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    Button button;

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATEANDEAT;  // ignore activation click
        case WM_MENUCHAR:
            return MAKELRESULT(0, MNC_CLOSE);  // ignore menu keys
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_SYSKEYUP:
        case WM_INPUT:
        case WM_MOUSEHOVER:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            break;
        case WM_SYSKEYDOWN:
            if (PhysicalInput::get().getMouseMode() == MouseMode::Absolute) {
                PhysicalInput::get().setMouseMode(MouseMode::Relative);
                window->centerCursor();
            } else {
                PhysicalInput::get().setMouseMode(MouseMode::Absolute);
            }
            break;
        case WM_LBUTTONDOWN: {
            Button button = Button::MOUSE_LB;

            PhysicalInput::get().buttonPressed(Button::MOUSE_LB);
            break;
        }
        case WM_RBUTTONDOWN: {
            Button button = Button::MOUSE_RB;

            PhysicalInput::get().buttonPressed(button);
            break;
        }
        case WM_MBUTTONDOWN: {
            Button button = Button::MOUSE_MB;

            PhysicalInput::get().buttonPressed(button);
            break;
        }
        case WM_KEYDOWN: {
            Button button = button_mapping[wParam];

            PhysicalInput::get().buttonPressed(button);
            break;
        }
        case WM_LBUTTONUP: {
            Button button = Button::MOUSE_LB;

            PhysicalInput::get().buttonReleased(button);
            break;
        }
        case WM_RBUTTONUP: {
            Button button = Button::MOUSE_RB;

            PhysicalInput::get().buttonReleased(button);
            break;
        }
        case WM_MBUTTONUP: {
            Button button = Button::MOUSE_MB;

            PhysicalInput::get().buttonReleased(button);
            break;
        }
        case WM_KEYUP: {
            Button button = button_mapping[wParam];

            PhysicalInput::get().buttonReleased(button);
            break;
        }
        case WM_MOUSEMOVE: {
            if (PhysicalInput::get().getMouseMode() == MouseMode::Absolute)
                break;

            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int center_x_offset = window->width / 2;
            int center_y_offset = window->height / 2;

            float relative_x_position =
                static_cast<float>(xPos - center_x_offset) / window->width;
            float relative_y_position =
                static_cast<float>(yPos - center_y_offset) / window->height;

            PhysicalInput::get().axisUpdated(Axis::MOUSE_X,
                                             relative_x_position);
            PhysicalInput::get().axisUpdated(Axis::MOUSE_Y,
                                             relative_y_position);

            window->centerCursor();
        } break;
        case WM_SIZE:
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::processMessages() {
    MSG message = {};

    bool got_message = PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE);
    if (got_message == false) return true;

    TranslateMessage(&message);
    DispatchMessage(&message);

    if (message.message == WM_QUIT) return false;
    return true;
}

void Window::registerWindowClass() {
    WNDCLASS wnd = {};
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hInstance = NULL;
    wnd.lpfnWndProc = Window::WndProc;
    wnd.lpszClassName = WINDOW_CLASS_NAME;
    wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wnd);
}

RECT Window::createWindowRect(DWORD style) {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;

    AdjustWindowRect(&rect, style, false);

    return rect;
}

void Window::init(const WindowConfig& config) {
    constexpr auto WINDOW_STYLE = WS_OVERLAPPEDWINDOW;

    width = config.width;
    height = config.height;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ImGui_ImplWin32_EnableDpiAwareness();

    registerWindowClass();

    auto rect = createWindowRect(WINDOW_STYLE);
    auto rect_width = rect.right - rect.left;
    auto rect_height = rect.bottom - rect.top;

    handle = CreateWindow(WINDOW_CLASS_NAME, "", WINDOW_STYLE, 0, 0, rect_width,
                          rect_height, HWND_DESKTOP, NULL, NULL, NULL);

    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(handle, SW_SHOW);

    centerCursor();
}

void Window::centerCursor() {
    int center_x = width / 2;
    int center_y = height / 2;

    POINT point = {center_x, center_y};
    ClientToScreen(handle, &point);
    SetCursorPos(point.x, point.y);
}
