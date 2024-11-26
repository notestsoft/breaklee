local timer = 0

function on_spawn(runtime, world, mob)
    print("Let's Beyblade!")
    timer = os.time()
end

function on_damage(runtime, world, mob, damage)
end

function on_update(runtime, world, mob)
    if os.time() - timer >= 1 then
        x = mob_pos_x(mob) + math.random(-1, 1)
        y = mob_pos_y(mob) + math.random(-1, 1)
    
        mob_set_pos(runtime, mob, x, y)    
    end
end

function on_despawn(runtime, world, mob)
end
