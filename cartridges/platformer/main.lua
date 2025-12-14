-- ULICS Demo: PLATFORMER
-- Simple platformer - collect all coins!
-- Controls: Arrow keys to move, Z to jump

-- Button constants
local BTN_LEFT = 0
local BTN_RIGHT = 1
local BTN_A = 4

-- Player
local px = 32
local py = 200
local pvx = 0
local pvy = 0
local on_ground = false

-- Physics
local GRAVITY = 0.3
local JUMP = -5
local SPEED = 2

-- Coins
local coins = {}
local collected = 0

-- DEBUG
local jump_attempts = 0
local last_jump_frame = 0
local frame_count = 0

function _init()
    -- Place coins
    add_coin(80, 180)
    add_coin(140, 150)
    add_coin(200, 120)
    add_coin(120, 80)
    add_coin(60, 60)
end

function _update()
    frame_count = frame_count + 1

    -- Horizontal movement
    if btn(BTN_LEFT) then
        pvx = -SPEED
    elseif btn(BTN_RIGHT) then
        pvx = SPEED
    else
        pvx = pvx * 0.7 -- Friction
    end

    -- Jump with DEBUG
    if btnp(BTN_A) and on_ground then
        pvy = JUMP
        on_ground = false
        jump_attempts = jump_attempts + 1
        last_jump_frame = frame_count
    end

    -- Gravity
    pvy = pvy + GRAVITY
    if pvy > 8 then pvy = 8 end

    -- Update position
    px = px + pvx
    py = py + pvy

    -- RESET grounded state (will be set by collisions below)
    on_ground = false

    -- Floor collision (ONLY when FALLING, not when jumping UP!)
    if py >= 216 and pvy >= 0 then
        py = 216
        pvy = 0
        on_ground = true
    end

    -- Platform collisions
    check_plat(64, 192, 48, 8)
    check_plat(140, 160, 48, 8)
    check_plat(100, 128, 56, 8)
    check_plat(180, 96, 48, 8)
    check_plat(40, 64, 72, 8)

    -- Screen bounds
    if px < 0 then px = 0 end
    if px > 248 then px = 248 end

    -- Collect coins
    for i = #coins, 1, -1 do
        local c = coins[i]
        local dx = px + 4 - c.x
        local dy = py + 4 - c.y
        -- Simple box collision
        if dx > -8 and dx < 8 and dy > -8 and dy < 8 then
            table.remove(coins, i)
            collected = collected + 1
        end
    end

    -- Win and restart
    if collected >= 5 and btnp(BTN_A) then
        px = 32
        py = 200
        pvx = 0
        pvy = 0
        coins = {}
        collected = 0
        add_coin(80, 180)
        add_coin(140, 150)
        add_coin(200, 120)
        add_coin(120, 80)
        add_coin(60, 60)
    end
end

function _draw()
    cls(12) -- Sky blue

    -- Draw platforms
    draw_plat(64, 192, 48, 8)
    draw_plat(140, 160, 48, 8)
    draw_plat(100, 128, 56, 8)
    draw_plat(180, 96, 48, 8)
    draw_plat(40, 64, 72, 8)

    -- Floor
    rectfill(0, 224, 256, 32, 3)

    -- Coins
    if coins then
        for i = 1, #coins do
            local c = coins[i]
            if c and c.x and c.y then
                rectfill(c.x - 3, c.y - 3, 6, 6, 10)
                rect(c.x - 3, c.y - 3, 6, 6, 9)
            end
        end
    end

    -- Player
    local draw_px = flr(px)
    local draw_py = flr(py)

    rectfill(draw_px, draw_py, 8, 8, 8)
    rectfill(draw_px + 2, draw_py - 2, 4, 2, 14)
    pset(draw_px + 2, draw_py - 1, 0)
    pset(draw_px + 5, draw_py - 1, 0)

    -- UI
    print("COINS: " .. collected .. "/5", 4, 4, 7)

    if collected >= 5 then
        print("YOU WIN!", 100, 120, 11)
        print("PRESS Z TO PLAY AGAIN", 55, 135, 7)
    end

    -- === DEBUG HUD ===
    local dx = 130
    local dy = 4

    -- Ground state (GREEN if true, RED if false)
    local gcolor = on_ground and 11 or 8
    print("GROUND: " .. tostring(on_ground), dx, dy, gcolor)
    dy = dy + 8

    -- Position
    print("PY: " .. flr(py), dx, dy, 6)
    dy = dy + 8

    -- Velocity
    print("VY: " .. flr(pvy * 10) / 10, dx, dy, 6)
    dy = dy + 8

    -- Button states
    if btn(BTN_LEFT) then
        print("LEFT", dx, dy, 11)
        dy = dy + 8
    end
    if btn(BTN_RIGHT) then
        print("RIGHT", dx, dy, 11)
        dy = dy + 8
    end
    if btn(BTN_A) then
        print("Z-DOWN", dx, dy, 11)
        dy = dy + 8
    end

    -- Big flash when btnp detects
    if btnp(BTN_A) then
        print("*** Z PRESS! ***", 50, 90, 10)
    end

    print("ARROWS: MOVE  Z: JUMP", 50, 245, 6)
end

function check_plat(x, y, w, h)
    -- AABB collision
    if px + 8 > x and px < x + w and
        py + 8 > y and py < y + h then
        -- Landing from above (ONLY if moving DOWN, not jumping up!)
        if pvy > 0 and py < y + 4 then
            py = y - 8
            pvy = 0
            on_ground = true
        end
        -- REMOVED: Don't touch pvy if jumping upward (pvy < 0)
    end
end

function draw_plat(x, y, w, h)
    rectfill(x, y, w, h, 4)
    line(x, y, x + w, y, 6)
end

function add_coin(x, y)
    table.insert(coins, { x = x, y = y })
end
