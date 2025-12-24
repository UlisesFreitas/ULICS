-- TEST: Inicializar platforms SIN _init()

-- DECLARAR E INICIALIZAR EN EL MISMO LUGAR
platforms = {
    { x = 40,  y = 200, w = 6 },
    { x = 100, y = 160, w = 5 },
    { x = 160, y = 120, w = 4 }
}

player = { x = 64, y = 100, vx = 0, vy = 0, on_ground = false }

GRAVITY = 0.3
JUMP = -5
SPEED = 2

-- NO usar _init(), dejar vacio o no definirlo
function _init()
    -- Vacio a proposito
end

function _update()
    -- Movement
    if btn(0) then player.vx = -SPEED end
    if btn(1) then player.vx = SPEED end
    if not (btn(0) or btn(1)) then player.vx = player.vx * 0.8 end

    -- Jump
    if btnp(4) and player.on_ground then
        player.vy = JUMP
        player.on_ground = false
    end

    -- Gravity
    player.vy = player.vy + GRAVITY
    if player.vy > 8 then player.vy = 8 end

    -- Update
    player.x = player.x + player.vx
    player.y = player.y + player.vy

    -- Reset ground
    player.on_ground = false

    -- Floor
    if player.y >= 240 then
        player.y = 240
        player.vy = 0
        player.on_ground = true
    end

    -- Platform collisions usando FLAG
    for i = 1, #platforms do
        local p = platforms[i]
        local pw = p.w * 8

        if fget(1, 1) then -- Verificar FLAG 1
            if player.x + 8 > p.x and player.x < p.x + pw and
                player.y + 8 > p.y and player.y < p.y + 8 then
                if player.vy > 0 and player.y < p.y + 4 then
                    player.y = p.y - 8
                    player.vy = 0
                    player.on_ground = true
                end
            end
        end
    end

    -- Bounds
    if player.x < 0 then player.x = 0 end
    if player.x > 248 then player.x = 248 end
end

function _draw()
    cls(12)

    -- Draw platforms
    for i = 1, #platforms do
        local p = platforms[i]
        -- Dibujar con rectangles Y sprites
        rectfill(p.x, p.y, p.w * 8, 8, 4)
        for tx = 0, p.w - 1 do
            spr(1, p.x + tx * 8, p.y)
        end
    end

    -- Draw player
    spr(0, flr(player.x), flr(player.y))

    -- Floor
    rectfill(0, 248, 256, 8, 3)

    -- UI
    print("ARROWS: MOVE  Z: JUMP", 60, 4, 7)
    print("PLATFORMS: " .. #platforms, 4, 16, 11)
    print("GROUND: " .. tostring(player.on_ground), 4, 24, player.on_ground and 11 or 8)

    -- Flags working?
    print("FLAGS LOADED:", 4, 40, 7)
    print("  Sprite 0 flag 0: " .. tostring(fget(0, 0)), 4, 48, fget(0, 0) and 11 or 8)
    print("  Sprite 1 flag 1: " .. tostring(fget(1, 1)), 4, 56, fget(1, 1) and 11 or 8)
end
