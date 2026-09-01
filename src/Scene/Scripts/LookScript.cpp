#include "LookScript.h"

#include <numbers>

#include "CameraData.h"
#include "GameInputContext.h"
#include "Graphics.h"
#include "Transform.h"
#include "WorldMatrix.h"

LookScript::LookScript(const Entity& head_entity, const Entity& player_entity,
                       const Camera& camera,
                       const std::shared_ptr<Input::GameInputContext>& input)
    : current_pitch(0),
      current_yaw(0),
      camera(camera),
      head_entity(head_entity),
      player_entity(player_entity),
      input(input),
      render_world(Graphics::getRenderEngine()->getRenderWorld()) {}

void LookScript::update(float delta_time) {
    ZoneScoped;
    float pitch = input->getAxis(Input::GameAxes::LookPitch);
    float yaw = input->getAxis(Input::GameAxes::LookYaw);

    current_pitch += pitch;
    current_yaw += yaw;

    constexpr auto PI = std::numbers::pi_v<float>;
    constexpr auto HALF_PI = PI / 2;
    constexpr auto TWO_PI = PI * 2;
    if (current_pitch > HALF_PI)
        current_pitch = HALF_PI;
    else if (current_pitch < -HALF_PI)
        current_pitch = -HALF_PI;

    if (current_yaw > PI)
        current_yaw -= TWO_PI;
    else if (current_yaw < -PI)
        current_yaw += TWO_PI;

    auto head_transform = head_entity.get<Transform>();
    head_transform->setOrientation(Quaternion(current_pitch, 0, 0));

    auto player_transform = player_entity.get<Transform>();
    player_transform->setOrientation(Quaternion(0, current_yaw, 0));
}

void LookScript::preRender() {
    ZoneScoped;
    auto projection = camera.getProjectionMatrix();

    Vector3 position;
    Quaternion orientation;
    Vector3 scale;
    head_entity.get<WorldMatrix>()->matrix.decompose(position, scale,
                                                     orientation);

    auto up = Vector3(0, 1, 0).rotate(orientation);
    auto forward = Vector3(0, 0, 1).rotate(orientation);
    auto right = Vector3(-1, 0, 0).rotate(orientation);

    Matrix view = Matrix::view(position, forward, up);
    Matrix view_camera_centered = Matrix::view(Vector3(), forward, up);

    CameraData data;
    data.view_projection = projection * view;
    data.view_projection_camera_centered = projection * view_camera_centered;
    data.right = right;
    data.up = up;

    render_world.setCameraData(data);
}
