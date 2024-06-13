
function on_spawn(runtime, world, mob)
     print("Hello, monster!")
end


function on_damage(runtime, world, mob, damage)
end

function on_despawn(runtime, world, mob)
    print("Goodbye, monster!")

    x = mob_pos_x(mob)
    y = mob_pos_y(mob)
    world_spawn_mob(runtime, world, 226, x, y)
end
