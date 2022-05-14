#pragma once
#include <Windows.h>


class Window
{
public:
	Window() {
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
		rect.right = 1920;
		rect.bottom = 1080;

		m_hWnd = CreateWindow(wnd.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, rect.right - rect.left, rect.bottom - rect.top, HWND_DESKTOP, NULL, NULL, NULL);
	}

	static Window& Get() {
		static Window instance;
		return instance;
	}

	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}

	void Run() {
		m_message = {};
		while (m_message.message != WM_QUIT) {
			if (PeekMessage(&m_message, NULL, NULL, NULL, PM_REMOVE)) {
				TranslateMessage(&m_message);
				DispatchMessage(&m_message);
			}
		}
	}

	inline HWND GetHandle() const { return m_hWnd; }
private:
	HWND m_hWnd;
	MSG m_message;
	int m_width;
	int m_height;
};