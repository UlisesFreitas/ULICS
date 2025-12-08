#ifndef EMBEDDED_SCRIPTS_H
#define EMBEDDED_SCRIPTS_H

namespace EmbeddedScripts {

// The content of our main.lua script is embedded here as a C++ raw string literal.
const char* DEMO_CART = R"lua(
-- This function is called 60 times per second for game logic.
function _update()
    -- Game logic will go here in the future.
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
end
)lua";

} // namespace EmbeddedScripts

#endif // EMBEDDED_SCRIPTS_H