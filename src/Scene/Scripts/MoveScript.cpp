#include "MoveScript.h"

#include <format>

#include "Overlay.h"
#include "Transform.h"
#include "Vector3.h"

MoveScript::MoveScript(const Entity& player,
                       const std::shared_ptr<Input::GameInputContext>& input)
    : player(player), input(input), speed(5) {
    Graphics::Overlay::get().add<Graphics::OverlayElements::Text>(
        "General/Player", [this]() {
            return std::format("Position is x:{} y:{} z:{}", position.x,
                               position.y, position.z);
        });
}

void MoveScript::update(float delta_time) {
    ZoneScoped;
    float forward_backward =
        input->getAxis(Input::GameAxes::MoveForwardBackward);
    float right_left = input->getAxis(Input::GameAxes::MoveLeftRight);
    float up_down = input->getAxis(Input::GameAxes::MoveUpDown);

    auto player_transform = player.get<Transform>();
    auto forward = player_transform->getForward();
    auto right = player_transform->getRight();
    auto up = player_transform->getUp();
    auto movement =
        forward * forward_backward + right * right_left + up * up_down;

    if (movement == Vector3()) return;
    movement = movement.normalized() * delta_time * speed;

    position = player_transform->getPosition() + movement;
    player_transform->setPosition(position);
}