function CreatePlayer()
    local player = World.createEntity()

    local camera = CreateCamera()
    player.addChild()

    return player
end

function CreateCamera()
    local camera = World.createEntity()
    Engine.setCameraEntity(camera)

    return camera
end
