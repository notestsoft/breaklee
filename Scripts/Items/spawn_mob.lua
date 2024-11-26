function OnEvent(Character)
    local MobSpeciesIndex = 4363
    local X = -1
    local Y = -1
    local Interval = 100000
    local Count = 1
    local MobPatternIndex = 0
    local MobScriptName = "Mobs/beyblade.lua"
    local Delay = 100

    Character:SpawnMob(
        MobSpeciesIndex, 
        X, 
        Y, 
        Interval,
        Count,
        MobPatternIndex, 
        MobScriptName,
        Delay
    )
end
