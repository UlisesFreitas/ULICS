#ifndef EMBEDDED_SCRIPTS_H
#define EMBEDDED_SCRIPTS_H

namespace EmbeddedScripts {

// The content of our main.lua script is embedded here as a C++ raw string literal.
const char* DEMO_CART = R"lua(
-- Player state table
local player = { x = 128, y = 128, color = 8 }

-- This function is called 60 times per second for game logic.
function _update()
    -- Player movement
    if btn(0) then player.x = player.x - 1 end -- Left
    if btn(1) then player.x = player.x + 1 end -- Right
    if btn(2) then player.y = player.y - 1 end -- Up
    if btn(3) then player.y = player.y + 1 end -- Down

    -- Change color with action buttons
    -- btnp (press) ensures the color only changes once per key press
    if btnp(4) then player.color = 9 end  -- Z for Orange
    if btnp(5) then player.color = 10 end -- X for Yellow

    -- Reset color if no action button is held
    if not btn(4) and not btn(5) then player.color = 8 end -- Red
end

-- This function is called once per frame for drawing.
function _draw()
    -- Use the 'clear' function we exposed from C++.
    clear(1) -- Clear the screen with dark blue.

    -- Test the new drawing functions
    rectfill(10, 10, 100, 80, 0)      -- Black filled rectangle
    rect(10, 10, 100, 80, 6)         -- Outline it in light gray

    line(15, 15, 105, 85, 8)         -- Draw a red diagonal line

    pset(128, 128, 7)                -- White pixel in the center
    pset(129, 128, 9)                -- Orange pixel to the right

    -- Test circle functions
    circfill(200, 50, 30, 12)        -- Blue filled circle
    circ(200, 50, 30, 7)             -- Outline it in white

    -- Use pget to get the color of the rect and draw a circle with it
    local rect_color = pget(15, 15)  -- Should be 0 (black)
    circfill(200, 150, 20, rect_color + 11) -- Draw a green circle

    -- Draw the player
    rectfill(player.x, player.y, 8, 8, player.color)    

    -- Test the print function by showing player coordinates
    print("X:"..player.x.." Y:"..player.y, 4, 4, 7)

    -- Test the time function with a pulsing circle
    local pulse = (time() * 2) % 2
    circ(50, 200, 10 + pulse * 10, 14)

    -- Test math functions with an orbiting moon
    local orbit_radius = 40
    local orbit_x = 200 + cos(time()/4) * orbit_radius
    local orbit_y = 50 + sin(time()/4) * orbit_radius
    circfill(flr(orbit_x), flr(orbit_y), 5, 6)
end
)lua";

} // namespace EmbeddedScripts

#endif // EMBEDDED_SCRIPTS_H