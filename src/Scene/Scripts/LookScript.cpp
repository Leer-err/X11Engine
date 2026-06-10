#include "LookScript.h"

#include "CameraData.h"
#include "GameInputContext.h"
#include "Graphics.h"
#include "Transform.h"
#include "WorldMatrix.h"

LookScript::LookScript(const Entity& head_entity, const Entity& player_entity,
                       const Camera& camera,
                       const std::shared_ptr<Input::GameInputContext>& input)
    : player_entity(player_entity),
      head_entity(head_entity),
      camera(camera),
      input(input),
      current_pitch(0),
      current_yaw(0),
      render_world(Graphics::getRenderEngine()->getRenderWorld()) {}

void LookScript::update(float delta_time) {
    ZoneScoped;
    float pitch = input->getAxis(Input::GameAxes::LookPitch);
    float yaw = input->getAxis(Input::GameAxes::LookYaw);

    current_pitch += pitch;
    current_yaw += yaw;

    if (current_pitch > 1.57f)
        current_pitch = 1.57f;
    else if (current_pitch < -1.57f)
        current_pitch = -1.57f;

    if (current_yaw > 3.14f)
        current_yaw -= 6.28f;
    else if (current_yaw < -3.14f)
        current_yaw += 6.28f;

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

    Matrix view = Matrix::view(position, forward, up);

    CameraData data;
    data.view_projection = projection * view;
    data.inverse_view_projection = data.view_projection.inverse();

    render_world.setCameraData(data);
}
