#include "OverlayRenderer.h"

#include "EngineData.h"
#include "Overlay.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"

namespace Graphics {

OverlayRenderer::OverlayRenderer() {}

void OverlayRenderer::render(const FrameData& frame_data) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    Overlay::Overlay::get().draw();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                    frame_data.cmd.buffer);
}

}  // namespace Graphics