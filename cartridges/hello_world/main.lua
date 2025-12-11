-- ULICS Cartridge: Hello World
-- A minimal example showing the basics

local message = "HELLO, ULICS!"
local x = 80
local y = 120
local color_index = 0

function _update()
    -- Cycle through colors slowly
    color_index = (color_index + 0.05) % 16
end

function _draw()
    cls(1)  -- Dark blue background
    
    -- Draw a welcome message
    print(message, x, y, flr(color_index))
    
    -- Draw some info
    print("PRESS ARROW KEYS", 60, 140, 7)
    print("TIME: " .. flr(time()), 90, 160, 6)
end
