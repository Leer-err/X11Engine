function Init()
    CreatePlayer()
end

function Update(delta_time)
    MovePlayer(delta_time)
    CameraLook()
end
