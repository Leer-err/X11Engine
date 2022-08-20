#include "Window.h"

Window::Window() : m_width(1920), m_height(1080) {
    m_keyboard = make_unique<DirectX::Keyboard>();

    WNDCLASS wnd = {};
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hInstance = NULL;
    wnd.lpfnWndProc = Window::WndProc;
    wnd.lpszClassName = "WindowClass";
    wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wnd);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = m_width;
    rect.bottom = m_height;

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false);

    m_hWnd =
        CreateWindow(wnd.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0,
                     0, rect.right - rect.left, rect.bottom - rect.top,
                     HWND_DESKTOP, NULL, NULL, NULL);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_SYSKEYDOWN:
            // if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
            // }
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_MOUSEMOVE: {
            POINT center;
            center.x = Window::get()->m_width / 2;
            center.y = Window::get()->m_height / 2;
            float x = (static_cast<float>(GET_X_LPARAM(lParam)) - center.x) /
                      Window::get()->m_width;
            float y = (static_cast<float>(GET_Y_LPARAM(lParam)) - center.y) /
                      Window::get()->m_height;
            EventManager::get()->RaiseEvent<MouseMove>(x, y);
            ClientToScreen(hWnd, &center);
            SetCursorPos(center.x, center.y);
        } break;
        case WM_MENUCHAR:
            return MAKELRESULT(0, MNC_CLOSE);
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::Run() {
    POINT center{m_width / 2, m_height / 2};
    ClientToScreen(m_hWnd, &center);
    SetCursorPos(center.x, center.y);
    m_running = true;
    MSG message = {};
    while (message.message != WM_QUIT) {
        if (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    Terminate();
}