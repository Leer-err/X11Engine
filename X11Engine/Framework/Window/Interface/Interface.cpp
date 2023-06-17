#include "Interface.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <implot.h>

#include <algorithm>

Interface::Interface(ID3D11Device* device, ID3D11DeviceContext* context,
                     HWND hWnd)
    : m_isShowing(false), m_frameTimes(1000, 0.f) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, context);
}

Interface::~Interface() {
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void Interface::Update() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGuiIO& io = ImGui::GetIO();

    if (m_isShowing) {
        ImGui::NewFrame();

        float frameTime = 1000.0f / ImGui::GetIO().Framerate;
        m_frameTimes.push_back(frameTime);
        m_frameTimes.pop_front();

        float frameTimes[1000];
        for (int i = 0; i < 1000; i++) {
            frameTimes[i] = m_frameTimes[i];
        }

        ImGui::Begin("Debug");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", frameTime,
                    ImGui::GetIO().Framerate);

        if (ImPlot::BeginPlot("##")) {
            auto bounds =
                minmax_element(m_frameTimes.begin(), m_frameTimes.end());
            ImPlot::SetupAxisLimits(ImAxis_Y1, *bounds.first, *bounds.second,
                                    ImPlotCond_Always);
            ImPlot::PlotLine("##", frameTimes, 1000, 1.f, 0,
                             ImPlotLineFlags_NoClip);
            ImPlot::EndPlot();
        }

        ImGui::End();

        ImGui::Begin("Console");

        // ImGui::LogText();

        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}
