#ifndef ULICS_EMBEDDED_CARTRIDGE_H
#define ULICS_EMBEDDED_CARTRIDGE_H

#include <string_view>


namespace Ulics {
namespace EmbeddedCartridge {

// --- Embedded config.json for the boot cartridge ---
constexpr std::string_view DEMO_CONFIG_JSON = R"json(
{
  "title": "ULICS Boot",
  "author": "ULICS Team",
  "version": "1.0.0",
  "description": "The ULICS boot cartridge.",
  "config": {
    "palette_size": 16,
    "memory_limit_mb": 1024,
    "lua_code_limit_lines": 262144
  }
}
)json"; 

// --- Embedded main.lua for the boot cartridge ---
constexpr std::string_view DEMO_LUA_SCRIPT = R"lua(
function _draw()
    -- Set black (0) as the transparent color
    tcolor(0)

    -- Use the 'clear' function we exposed from C++.
    clear(1) -- Clear the screen with dark blue.

    -- Disable transparency to draw black text
    tcolor(nil)
    print("ULICS CONSOLE", 88, 128, 0)
end
)lua";
} // namespace EmbeddedCartridge
} // namespace Ulics
#endif // ULICS_EMBEDDED_CARTRIDGE_H