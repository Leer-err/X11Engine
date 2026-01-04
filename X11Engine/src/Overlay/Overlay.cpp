#include "Overlay.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>
#include <winscard.h>

#include <ranges>
#include <string>

#include "APIResources.h"
#include "Renderer.h"
#include "Window.h"

Overlay::OverlayElements::Header::Header(const std::string& label)
    : label(label) {}

void Overlay::OverlayElements::Header::draw() {
    if (ImGui::CollapsingHeader(label.c_str())) {
        for (auto& child : elements) {
            std::visit([](auto& value) { value.draw(); }, child);
        }

        for (auto& [name, child] : headers) {
            child.draw();
        }
    }
}

Overlay::OverlayElements::SliderFloat::SliderFloat(
    const std::string& label, std::function<void(float)> callback, float min,
    float max, float initial)
    : label(label),
      callback(callback),
      minimal(min),
      maximum(max),
      value(initial) {}

void Overlay::OverlayElements::SliderFloat::draw() {
    bool is_changed =
        ImGui::SliderFloat(label.c_str(), &value, minimal, maximum);

    if (is_changed) callback(value);
}

Overlay::Overlay::Overlay() {
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));

    float framebuffer_x_scale = static_cast<float>(Renderer::get().getWidth()) /
                                Window::get().getWidth();
    float framebuffer_y_scale =
        static_cast<float>(Renderer::get().getHeight()) /
        Window::get().getHeight();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplayFramebufferScale.x = framebuffer_x_scale;
    io.DisplayFramebufferScale.y = framebuffer_y_scale;

    ImGuiStyle& style = ImGui::GetStyle();
    style.FontSizeBase = 30.0f;

    auto window_handle = Window::get().getHandle();
    ImGui_ImplWin32_Init(window_handle);

    auto device = APIResources::get().getDevice().Get();
    auto context = APIResources::get().getContext().Get();
    ImGui_ImplDX11_Init(device, context);
}

Overlay::Overlay::~Overlay() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Overlay::Overlay::draw() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Demo", nullptr, 0);

    for (auto& child : elements) {
        std::visit([](auto& value) { value.draw(); }, child);
    }

    for (auto& [name, child] : headers) {
        child.draw();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}