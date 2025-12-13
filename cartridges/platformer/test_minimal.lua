-- MINIMAL PLATFORMER TEST
-- Debug version to isolate the bug

px = 120
py = 120

function _init()
    print("Init OK")
end

function _update()
    -- Simple movement
    if btn(1) then px = px + 2 end -- RIGHT
    if btn(0) then px = px - 2 end -- LEFT

    -- Keep on screen
    if px < 0 then px = 0 end
    if px > 248 then px = 248 end
end

function _draw()
    cls(1) -- Dark blue

    -- Draw player
    rectfill(px, py, 8, 8, 11) -- Green square

    -- Debug info
    print("X: " .. px, 4, 4, 7)
end
