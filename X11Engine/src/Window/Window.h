#pragma once

#include <Windows.h>

#include <atomic>
#include <memory>

using std::atomic;
using std::make_unique;
using std::unique_ptr;

class IInput;

class Window {
   public:
    Window();

    void init(uint32_t width, uint32_t height);

    void setInput(std::shared_ptr<IInput> input);

    bool processMessages();

    inline HWND getHandle() const { return handle; }
    inline int getWidth() const { return width; }
    inline int getHeight() const { return height; }

   private:
    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void registerWindowClass();
    RECT createWindowRect(DWORD style);

    HWND handle;
    int width;
    int height;
};