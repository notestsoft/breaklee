
function on_spawn(runtime, world, mob)
    print("Greetings from another monster!")
end

function on_despawn(runtime, world, mob)
    print("Bye bye")
end

function on_damage(runtime, world, mob, damage)
    local x = mob_pos_x(mob) + math.random(-1, 2)
    local y = mob_pos_y(mob) + math.random(-1, 2)
    world_spawn_item(runtime, world, mob_id(mob), 13, damage, x, y)

    if math.random(0, 1000) < 300 then
        x = mob_pos_x(mob) + math.random(-1, 2)
        y = mob_pos_y(mob) + math.random(-1, 2)
        world_spawn_item(runtime, world, mob_id(mob), 1, 0, x, y)
    end 
    
    mob_ids = {227, 228, 229, 230}
    for k,v in pairs(names) do 
        if mob_id(mob) ~= v then
            world_despawn_mob(runtime, world, v)
        end
    end
end
