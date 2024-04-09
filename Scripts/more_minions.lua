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

function on_despawn(runtime, world, mob)
    spawn_minion(runtime, world, mob, 227)
    spawn_minion(runtime, world, mob, 228)
    spawn_minion(runtime, world, mob, 229)
    spawn_minion(runtime, world, mob, 230)
end
