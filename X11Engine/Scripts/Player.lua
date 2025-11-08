function CreatePlayer()
    local player = World.createEntity()

    local camera = CreateCamera()
    player.addChild()

    return player
end

function CreateCamera()
    local camera = World.createEntity()
    CameraManager.setCameraEntity(camera)

    return camera
end
