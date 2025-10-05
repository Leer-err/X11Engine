#include "OverlayRenderSystem.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>

#include <format>
#include <memory>
#include <string>
#include <tracy/Tracy.hpp>
#include <vector>

#include "Children.h"
#include "Dx11Factory.h"
#include "Entity.h"
#include "GameInputContext.h"
#include "IResourceFactory.h"
#include "Name.h"
#include "Parent.h"
#include "PhysicalInput.h"
#include "PhysicsComponents.h"
#include "PositionComponents.h"
#include "Window.h"
#include "World.h"

constexpr int INITIAL_MAX_FRAMETIMES = 100;

OverlayRenderSystem::OverlayRenderSystem(
    std::shared_ptr<Window> window,
    std::shared_ptr<IResourceFactory> resource_factory)
    : is_showing(false), max_frame_times(INITIAL_MAX_FRAMETIMES) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Dx11Factory* dx_factory = static_cast<Dx11Factory*>(resource_factory.get());
    ID3D11Device* device_handle = dx_factory->getDeviceHandle();
    ID3D11DeviceContext* context_handle = dx_factory->getContextHandle();

    ImGui_ImplWin32_Init(window->getHandle());
    ImGui_ImplDX11_Init(device_handle, context_handle);
}

OverlayRenderSystem::~OverlayRenderSystem() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool OverlayRenderSystem::prepare() { return true; }

void OverlayRenderSystem::update(World& world, float delta_time) {
    ZoneScoped;

    ButtonState state = GameInputContext::get().getButton(TOGGLE_OVERLAY);

    if (state == ButtonState::Pressed) is_showing = !is_showing;

    if (is_showing == false) return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

    ImGui::Begin("Demo", nullptr, 0);

    drawHeader(delta_time);
    drawWorldInfo(world);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void OverlayRenderSystem::drawHeader(float time) {
    while (frame_times.size() >= max_frame_times) {
        frame_times.erase(frame_times.begin());
    }
    frame_times.push_back(time * 1000);
    ImGui::PlotHistogram("Frame Times", frame_times.data(), frame_times.size());
    ImGui::SliderInt("Frametimes max number", &max_frame_times, 10, 1000);
    ImGui::Text("FPS = %f", 1.f / time);
}

void OverlayRenderSystem::drawWorldInfo(World& world) {
    if (ImGui::CollapsingHeader("World")) {
        std::vector<Entity> entities =
            world.query().without<Parent>().execute();

        ImGui::Text("Total world entities = %lld", entities.size());

        for (const auto& entity : entities) {
            drawEntityInfo(entity);
        }
    }
}

void OverlayRenderSystem::drawEntityInfo(const Entity& entity) {
    std::string label = std::to_string(entity.getId());
    auto name = entity.get<Name>();

    if (name) {
        label = std::format("{} {}", entity.getId(), name->name);
    }

    if (ImGui::TreeNode(label.c_str())) {
        drawPositionInfo(entity);
        drawPhysicsInfo(entity);

        drawChildrenInfo(entity);

        ImGui::TreePop();
    }
}

void OverlayRenderSystem::drawPositionInfo(const Entity& entity) {
    auto global_matrix = entity.get<GlobalMatrix>();
    if (global_matrix) {
        ImGui::Text("%s", global_matrix->serialize().c_str());
    }
}

void OverlayRenderSystem::drawChildrenInfo(const Entity& entity) {
    auto children = entity.get<Children>();
    if (children) {
        ImGui::Text("Children:");
        for (const auto& child : children->children) {
            drawEntityInfo(child);
        }
    }
}

void OverlayRenderSystem::drawPhysicsInfo(const Entity& entity) {
    auto static_rigid_body = entity.get<StaticRigidBodyComponent>();
    if (static_rigid_body) {
        ImGui::Text("%s", static_rigid_body->serialize().c_str());
    }

    auto velocity = entity.get<Velocity>();
    if (velocity) {
        ImGui::Text("%s", velocity->serialize().c_str());
    }
}
