function CreatePlayer()
    Player = World.createEntity()
    Player:setName("Player")
    Player:addComponent(Components.Transform)

    Camera = World.createEntity()
    Camera:setName("Camera")
    Camera:addComponent(Components.Transform)

    local camera_transform = Camera:getTransform()
    camera_transform:setPosition(Vector3:new(0, 0, 0))
    Player:addChild(Camera)

    CameraManager.setCameraEntity(Camera)
end
