function MovePlayer(delta_time)
    local forward = GameInput.getAxis(GameAxis.MOVE_FORWARD_BACKWARD)
    local right = GameInput.getAxis(GameAxis.MOVE_RIGHT_LEFT)

    local player_transform = Player:getTransform()

    local current_position = player_transform:getPosition()

    local movement = (player_transform:getForward() * forward + player_transform:getRight() * right) * delta_time
    print(movement.x, movement.y, movement.z)

    local new_position = current_position + movement
    player_transform:setPosition(new_position)
end
