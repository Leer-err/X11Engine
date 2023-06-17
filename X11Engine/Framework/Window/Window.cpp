#include "Window.h"

#include <backends/imgui_impl_win32.h>

#include "./Interface/Interface.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

Window::Window() : m_width(1920), m_height(1080) {
    m_keyboard = make_unique<DirectX::Keyboard>();
    m_mouse = make_unique<DirectX::Mouse>();

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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

    m_mouse->SetWindow(m_hWnd);
    m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATEANDEAT;  // ignore activation click
        case WM_MENUCHAR:
            return MAKELRESULT(0, MNC_CLOSE);  // ignore menu keys
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_SYSKEYDOWN:
            if (wParam == VK_MENU) {
                DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
            }
            if (wParam == VK_F3 && (lParam & 0xffff) == 1) {
                Interface::get()->Toggle();
            }
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_SYSKEYUP:
            if (wParam == VK_MENU && lParam == 1) {
                DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
            }
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_F3 && (lParam & 0xffff) == 1) {
                Interface::get()->Toggle();
            }
        case WM_KEYUP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;
        case WM_INPUT:
        case WM_MOUSEHOVER:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
            break;
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