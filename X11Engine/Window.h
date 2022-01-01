#pragma once
#include <Windows.h>
#include "Graphics.h"

class Window
{
public:
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
private:
	int m_width;
	int m_height;
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS wnd = {};
	wnd.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = Window::WndProc;
	wnd.lpszClassName = L"WindowClass";
	wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wnd.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wnd);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 1920;
	rect.bottom = 1080;

	HWND hWnd = CreateWindow(wnd.lpszClassName, L"", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, rect.right - rect.left, rect.bottom - rect.top, HWND_DESKTOP, NULL, hInstance, NULL);

	Graphics g{ 1920, 1080, hWnd };

	vector<float> vert = {
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f
	};
	vector<uint32_t> ind = { 
		0, 2, 1,
		1, 2, 3
	};

	MSG message = {};
	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, HWND_DESKTOP, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		g.PreFrame();
		g.Draw(vert, ind);
		g.PostFrame();
	}
	return 0;
}