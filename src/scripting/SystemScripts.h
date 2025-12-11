#ifndef SYSTEM_SCRIPTS_H
#define SYSTEM_SCRIPTS_H

#include <string>

/**
 * @brief System scripts for ULICS internal functionality.
 * 
 * These scripts are embedded in the engine and provide core functionality
 * like the cartridge menu system.
 */
namespace SystemScripts {

/**
 * @brief ULICS System Menu
 * 
 * This is the main menu shown when ULICS starts without a cartridge specified.
 * It scans for available cartridges and allows the user to select one to load.
 * 
 * Features:
 * - Automatic cartridge discovery
 * - Keyboard navigation (arrow keys)
 * - Cartridge metadata display
 * - Load selected cartridge with Enter
 * 
 * This menu demonstrates ULICS's flexibility - it's just another Lua script,
 * but with special system functions exposed.
 */
const std::string MENU_SCRIPT = R"(
-- ================================================
-- ULICS SYSTEM MENU
-- ================================================
-- Main cartridge selection interface
-- 
-- Controls:
--   UP/DOWN   - Navigate cartridges
--   ENTER     - Load selected cartridge
--   ESC       - Quit

local cartridges = {}
local selected_index = 1
local scroll_offset = 0
local max_visible = 10

-- Menu colors
local COLOR_BG = 1        -- Dark blue
local COLOR_TITLE = 7     -- White
local COLOR_TEXT = 6      -- Light gray
local COLOR_SELECTED = 11 -- Green
local COLOR_HIGHLIGHT = 8 -- Red
local COLOR_INFO = 13     -- Light blue

-- Initialize menu
function _init()
    -- Get list of available cartridges
    -- NOTE: This function will be implemented in tasks 4.9-4.10
    -- For now, we'll use a placeholder
    cartridges = get_cartridge_list()
    
    if #cartridges == 0 then
        print("No cartridges found!")
    end
end

function _update()
    -- Navigate up
    if btnp(2) then  -- Up arrow
        selected_index = selected_index - 1
        if selected_index < 1 then
            selected_index = #cartridges
        end
        
        -- Adjust scroll
        if selected_index < scroll_offset + 1 then
            scroll_offset = selected_index - 1
        end
    end
    
    -- Navigate down
    if btnp(3) then  -- Down arrow
        selected_index = selected_index + 1
        if selected_index > #cartridges then
            selected_index = 1
        end
        
        -- Adjust scroll
        if selected_index > scroll_offset + max_visible then
            scroll_offset = selected_index - max_visible
        end
    end
    
    -- Load cartridge
    if btnp(4) then  -- Enter/Z key
        if #cartridges > 0 then
            local cart = cartridges[selected_index]
            load_cartridge(cart.path)
        end
    end
end

function _draw()
    cls(COLOR_BG)
    
    -- Draw header
    print("ULICS FANTASY CONSOLE", 40, 8, COLOR_TITLE)
    print("HIGH-PERFORMANCE EDITION", 35, 16, COLOR_INFO)
    line(0, 24, 255, 24, COLOR_TITLE)
    
    -- Draw stats
    local stats_y = 32
    print("CARTRIDGES: " .. #cartridges, 8, stats_y, COLOR_TEXT)
    print("512MB-1GB RAM | 1M LINES LUA", 8, stats_y + 8, COLOR_TEXT)
    line(0, stats_y + 16, 255, stats_y + 16, COLOR_TEXT)
    
    -- Draw cartridge list
    if #cartridges == 0 then
        print("NO CARTRIDGES FOUND", 60, 120, COLOR_HIGHLIGHT)
        print("Place .lua files in cartridges/", 30, 140, COLOR_TEXT)
        return
    end
    
    local list_y = stats_y + 24
    local line_height = 12
    
    for i = 1, math.min(max_visible, #cartridges) do
        local cart_index = scroll_offset + i
        if cart_index > #cartridges then break end
        
        local cart = cartridges[cart_index]
        local y = list_y + (i - 1) * line_height
        
        -- Highlight selected
        if cart_index == selected_index then
            rectfill(4, y - 1, 251, y + line_height - 3, COLOR_SELECTED)
            print("> " .. cart.name, 8, y, COLOR_BG)
        else
            print("  " .. cart.name, 8, y, COLOR_TEXT)
        end
    end
    
    -- Draw selected cartridge info
    if selected_index > 0 and selected_index <= #cartridges then
        local cart = cartridges[selected_index]
        local info_y = 200
        
        line(0, info_y - 4, 255, info_y - 4, COLOR_TITLE)
        print("SELECTED:", 8, info_y, COLOR_TITLE)
        print(cart.name, 8, info_y + 8, COLOR_TEXT)
        
        if cart.author then
            print("BY: " .. cart.author, 8, info_y + 16, COLOR_TEXT)
        end
    end
    
    -- Draw controls
    print("UP/DOWN: SELECT  ENTER: LOAD", 30, 240, COLOR_INFO)
end

-- Placeholder function - will be replaced by C++ API
function get_cartridge_list()
    -- This will be implemented in tasks 4.9-4.10
    -- For now, return a sample list for testing
    return {
        {name = "Hello World", path = "cartridges/hello_world", author = "ULICS Team"},
        {name = "Bouncing Balls", path = "cartridges/bouncing_ball", author = "ULICS Team"},
        {name = "[Load your own cartridges here]", path = "", author = ""}
    }
end

-- Placeholder function - will be replaced by C++ API
function load_cartridge(path)
    -- This will be implemented in task 4.9
    print("Loading: " .. path)
end

-- Initialize on load
_init()
)";

} // namespace SystemScripts

#endif // SYSTEM_SCRIPTS_H
