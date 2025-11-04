function MovePlayer()
    local forward = GameInput.isPressed(GameAction.TOGGLE_OVERLAY)

    if forward == true then
        local entity = World.createEntity()
        print(entity.id)
    end
end
