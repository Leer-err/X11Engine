#pragma once
#include <Windows.h>
#include <atomic>
#include <windowsx.h>

#include "Event/EventManager.h"

using std::atomic;

class Window
{
public:
	inline static Window* get() {
		static Window instance;
		return &instance;
	}

	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			EventManager::get()->RaiseEvent<KeyDown>(wParam);
			break;
		case WM_KEYUP:
			EventManager::get()->RaiseEvent<KeyUp>(wParam);
			break;
		case WM_MOUSEMOVE:
			{
			POINT center;
			center.x = Window::get()->m_width / 2;
			center.y = Window::get()->m_height / 2;
			float x = (static_cast<float>(GET_X_LPARAM(lParam)) - center.x) / Window::get()->m_width;
			float y = (static_cast<float>(GET_Y_LPARAM(lParam)) - center.y) / Window::get()->m_height;
			EventManager::get()->RaiseEvent<MouseMove>(x, y);
			ClientToScreen(hWnd, &center);
			SetCursorPos(center.x, center.y); 
			}
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}

	void Run() {
		POINT center {m_width/2, m_height/2};
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

	inline void Terminate() { m_running = false; }

	inline bool IsRunning() const { return m_running; }

	inline HWND GetHandle() const { return m_hWnd; }
	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }
private:
	Window() : m_width(1920), m_height(1080) {
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

		m_hWnd = CreateWindow(wnd.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, rect.right - rect.left, rect.bottom - rect.top, HWND_DESKTOP, NULL, NULL, NULL);
	}

	HWND m_hWnd;
	int m_width;
	int m_height;

	atomic<bool> m_running;
};