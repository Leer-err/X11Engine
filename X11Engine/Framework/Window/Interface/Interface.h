#include <Windows.h>
#include <d3d11.h>

#include <deque>

using std::deque;

class Interface {
    inline static Interface* instance(ID3D11Device* device = nullptr,
                                      ID3D11DeviceContext* context = nullptr,
                                      HWND hwnd = 0) {
        static Interface instance{device, context, hwnd};
        return &instance;
    }

   public:
    inline static Interface* init(ID3D11Device* device,
                                  ID3D11DeviceContext* context, HWND hwnd) {
        return instance(device, context, hwnd);
    }
    inline static Interface* get() { return instance(); }

    void Update();

    inline void Hide() { m_isShowing = false; }
    inline void Show() { m_isShowing = true; }
    inline void Toggle() { m_isShowing = !m_isShowing; }

   private:
    Interface(ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd);
    ~Interface();

    bool m_isShowing;
    deque<float> m_frameTimes;
};