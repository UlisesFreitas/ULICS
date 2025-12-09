#ifndef ULICS_EMBEDDED_CARTRIDGE_H
#define ULICS_EMBEDDED_CARTRIDGE_H

#include <string_view>

namespace Ulics {
namespace EmbeddedCartridge {

// --- Embedded config.json for the default 'demo' cartridge ---
constexpr std::string_view DEMO_CONFIG_JSON = R"json(
{
  "title": "ULICS Demo",
  "author": "ULICS Team",
  "version": "1.0.0",
  "description": "A demonstration cartridge showcasing the ULICS API.",
  "config": {
    "palette_size": 16,
    "memory_limit_mb": 1024,
    "lua_code_limit_lines": 262144
  }
}
)json";

// --- Embedded main.lua for the default 'demo' cartridge ---
constexpr std::string_view DEMO_LUA_SCRIPT = R"lua(
-- Player state table
local player = { x = 128, y = 128, color = 8 }

-- This function is called 60 times per second for game logic.
function _update()
    -- Set the camera to follow the player, centering them on screen.
    camera(player.x - 128 + 4, player.y - 128 + 4)

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
    -- Set black (0) as the transparent color
    tcolor(0)

    -- Use the 'clear' function we exposed from C++.
    clear(1) -- Clear the screen with dark blue.

    -- Draw a large background rectangle to show the world is moving
    rectfill(-128, -128, 512, 512, 2)

    -- Draw the player
    rectfill(player.x, player.y, 8, 8, player.color)    

    -- Test the print function by showing player coordinates
    print("X:"..player.x.." Y:"..player.y, 4, 4, 7)

    -- Test math functions with an orbiting moon
    local orbit_radius = 40
    local orbit_x = 200 + cos(time()/4) * orbit_radius
    local orbit_y = 50 + sin(time()/4) * orbit_radius
    circfill(flr(orbit_x), flr(orbit_y), 5, 6)

    -- Disable transparency to draw black text
    tcolor(nil)
    print("ULICS CONSOLE", 88, 128, 0)
end
)lua";

} // namespace EmbeddedCartridge
} // namespace Ulics

#endif // ULICS_EMBEDDED_CARTRIDGE_H