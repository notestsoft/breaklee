function OnEvent(Character)
    local EntityIndex = 4444
    local ObjectType = 6
    local EntitySourceIndex = 2535
    local PositionX = 12
    local PositionY = 33
    local Hp = 1
    local Nation = 0
    local Status = 0
    local NpcIndex = 0

    Character:SpawnObject(EntityIndex, ObjectType, EntitySourceIndex, PositionX, PositionY, Hp, Nation, Status, NpcIndex)
end
