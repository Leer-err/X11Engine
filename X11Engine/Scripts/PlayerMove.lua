function MovePlayer(delta_time)
    local forward = GameInput.getAxis(GameAxis.MOVE_FORWARD_BACKWARD)
    local right = GameInput.getAxis(GameAxis.MOVE_RIGHT_LEFT)

    local player_transform = Player:getTransform()

    local current_position = player_transform:getPosition()

    local movement = (player_transform:getForward() * forward + player_transform:getRight() * right) * delta_time

    local new_position = current_position + movement
    player_transform:setPosition(new_position)
end

local player_yaw = 0
local player_pitch = 0

function CameraLook()
    local x = GameInput.getAxis(GameAxis.LOOK_X)
    local y = GameInput.getAxis(GameAxis.LOOK_Y)

    player_yaw = player_yaw + x
    player_pitch = player_pitch + y

    if player_yaw > 3.14 then
        player_yaw = player_yaw - 3.14
    end
    if player_yaw < -3.14 then
        player_yaw = player_yaw + 3.14
    end

    if player_pitch > 1.57 then
        player_pitch = 1.57
    end
    if player_pitch < -1.57 then
        player_pitch = -1.57
    end

    print(player_yaw, player_pitch)
end
