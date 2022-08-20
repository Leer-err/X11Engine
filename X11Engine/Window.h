#pragma once

// clang-format off
#include <Windows.h>
// clang-format on

#include <Keyboard.h>
#include <windowsx.h>

#include <atomic>
#include <memory>

#include "Event/EventManager.h"

using std::atomic;
using std::make_unique;
using std::unique_ptr;

class Window {
   public:
    inline static Window* get() {
        static Window instance;
        return &instance;
    }

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void Run();

    inline void Terminate() { m_running = false; }

    inline bool IsRunning() const { return m_running; }

    inline HWND GetHandle() const { return m_hWnd; }
    inline int GetWidth() const { return m_width; }
    inline int GetHeight() const { return m_height; }

   private:
    Window();

    HWND m_hWnd;
    int m_width;
    int m_height;

    atomic<bool> m_running;

    unique_ptr<DirectX::Keyboard> m_keyboard;
};