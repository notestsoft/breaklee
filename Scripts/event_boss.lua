
function spawn_minion(runtime, world, mob, entity_id)
    --local r = 5 * math.sqrt(math.random())
    --local theta = math.random() * 2 * math.pi
    --local dx = r * math.cos(theta)
    --local dy = r * math.sin(theta)
    --local x = mob_pos_x(mob) + dx
    --local y = mob_pos_y(mob) + dy
    local x = mob_pos_x(mob) + math.random(-2, 2)
    local y = mob_pos_y(mob) + math.random(-2, 2)
    world_spawn_mob(runtime, world, entity_id, x, y)
end

local test_timer = 0
local spawned = 0

function on_spawn(runtime, world, mob)
    test_timer = os.time()
    print("Hello, monster!")
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
    
    spawn_minion(runtime, world, mob, 227)
    spawn_minion(runtime, world, mob, 228)
    spawn_minion(runtime, world, mob, 229)
    spawn_minion(runtime, world, mob, 230)
end

function on_update(runtime, world, mob)
--    if os.time() - test_timer > 10 then
--        test_timer = os.time()
--        world_despawn_mob(runtime, world, 226)
--    end
end

function on_despawn(runtime, world, mob)
end
