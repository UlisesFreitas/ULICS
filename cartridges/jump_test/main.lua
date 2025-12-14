-- INPUT DETECTION COMPARISON TEST
-- Compare btnp() in _update() vs _draw()

local py = 200
local pvy = 0
local on_ground = true
local jump_count = 0

local btnp_in_update = false
local btn_in_update = false

function _init()
end

function _update()
    -- Capture button state IN UPDATE
    btnp_in_update = btnp(4)
    btn_in_update = btn(4)

    -- Jump ONLY
    if btnp_in_update and on_ground then
        pvy = -5
        on_ground = false
        jump_count = jump_count + 1
    end

    -- Gravity
    pvy = pvy + 0.3

    -- Update position
    py = py + pvy

    -- Floor only
    if py >= 200 then
        py = 200
        pvy = 0
        on_ground = true
    end
end

function _draw()
    cls(1)

    -- Player
    rectfill(120, flr(py), 16, 16, 8)

    -- Floor
    rectfill(0, 216, 256, 40, 3)

    -- Debug
    local c = on_ground and 11 or 8
    print("GROUND: " .. tostring(on_ground), 10, 10, c)
    print("PY: " .. flr(py), 10, 20, 6)
    print("VY: " .. flr(pvy * 10) / 10, 10, 30, 6)
    print("JUMPS: " .. jump_count, 10, 40, 14)

    -- CRITICAL: Compare button detection
    print("--- IN _update() ---", 10, 60, 7)
    if btnp_in_update then
        print("btnp=TRUE", 10, 70, 11)
    else
        print("btnp=false", 10, 70, 8)
    end

    if btn_in_update then
        print("btn=TRUE", 10, 80, 11)
    else
        print("btn=false", 10, 80, 8)
    end

    print("--- IN _draw() ---", 10, 100, 7)
    if btnp(4) then
        print("btnp=TRUE", 10, 110, 11)
    else
        print("btnp=false", 10, 110, 8)
    end

    if btn(4) then
        print("btn=TRUE", 10, 120, 11)
    else
        print("btn=false", 10, 120, 8)
    end

    print("PRESS Z - WATCH BOTH", 60, 230, 7)
end
