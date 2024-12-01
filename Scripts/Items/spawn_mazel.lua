function OnEvent(Character)
    local MobSpeciesIndex = math.random(1, 4364)
    local X = -1
    local Y = -1
    local Interval = 100000
    local Count = 1
    local MobPatternIndex = math.random(0, 800)
    local MobScriptName = ""
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
