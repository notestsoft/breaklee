function OnEvent(Character)
    local EntityIndex = 1
    local ObjectType = 4
    local EntitySourceIndex = 1
    local PositionX = 20
    local PositionY = 33
    local Hp = 1
    local Nation = 0
    local Status = 1
    local NpcIndex = 0

    Character:SpawnObject(EntityIndex, ObjectType, EntitySourceIndex, PositionX, PositionY, Hp, Nation, Status, NpcIndex)
end
