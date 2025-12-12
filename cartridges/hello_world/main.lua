-- ULICS Cartridge: Hello World
-- A minimal example showing the basics

local message = "HELLO, ULICS!"
local x = 100
local y = 120
local color_index = 0

function _update()
    -- Cycle through colors slowly
    color_index = (color_index + 0.05) % 16

    -- Move with arrow keys
    if btn(0) then x = x - 2 end -- Left
    if btn(1) then x = x + 2 end -- Right
    if btn(2) then y = y - 2 end -- Up
    if btn(3) then y = y + 2 end -- Down

    -- Keep in bounds
    if x < 0 then x = 0 end
    if x > 200 then x = 200 end
    if y < 20 then y = 20 end
    if y > 230 then y = 230 end
end

function _draw()
    cls(1) -- Dark blue background

    -- Draw a welcome message
    print(message, x, y, flr(color_index))

    -- Draw some info
    print("USE ARROW KEYS TO MOVE!", 40, 10, 7)
    print("TIME: " .. flr(time()), 90, 240, 6)
end
