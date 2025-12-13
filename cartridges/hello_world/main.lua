-- ULICS Cartridge: Hello World
-- A minimal example showing the basics
-- Supports keyboard and gamepad!

local message     = "HELLO, ULICS!"
local x           = 100
local y           = 120
local color_index = 0

-- Button constants for clarity
local BTN_LEFT    = 0
local BTN_RIGHT   = 1
local BTN_UP      = 2
local BTN_DOWN    = 3
local BTN_A       = 4

function _update()
    -- Cycle through colors slowly
    color_index = (color_index + 0.05) % 16

    -- Move with arrow keys OR gamepad D-pad
    if btn(BTN_LEFT) then x = x - 2 end
    if btn(BTN_RIGHT) then x = x + 2 end
    if btn(BTN_UP) then y = y - 2 end
    if btn(BTN_DOWN) then y = y + 2 end

    -- Keep in bounds
    if x < 0 then x = 0 end
    if x > 200 then x = 200 end
    if y < 20 then y = 20 end
    if y > 230 then y = 230 end
end

function _draw()
    cls(1) -- Dark blue background

    -- Draw the welcome message
    print(message, x, y, flr(color_index))

    -- Instructions
    print("ARROW KEYS OR D-PAD TO MOVE!", 25, 10, 7)

    -- Optional: Show if gamepad detected
    -- (This would require a Lua API to query - not implemented yet)

    -- Time display
    print("TIME: " .. flr(time()), 90, 240, 6)
end
