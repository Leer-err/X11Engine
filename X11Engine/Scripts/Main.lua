function Init()
    local gem = World.createEntity()
    gem:addComponent(Components.StaticMesh)
    gem:addComponent(Components.Transform)

    local transform_component = gem:getComponent(Components.Transform)
    transform_component:setPosition(Vector3:new(0, 0, 5))

    local mesh_component = gem:getComponent(Components.StaticMesh)
    mesh_component:setMesh("./Assets/Gem.fbx")
end

function Update(delta_time)
    MovePlayer(delta_time)
    CameraLook()
end
