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
--   Z/X       - Load selected cartridge

local cartridges = {}
local selected_index = 1
local scroll_offset = 0
local max_visible = 10

local COLOR_BG = 1        -- Dark blue
local COLOR_TITLE = 7     -- White
local COLOR_TEXT = 6      -- Light gray
local COLOR_SELECTED = 11 -- Green
local COLOR_HIGHLIGHT = 8 -- Red
local COLOR_INFO = 13     -- Light blue

-- State variables
local last_input_time = 0
local INPUT_DELAY = 0.2  -- 200ms delay between inputs

-- Initialize menu
function _init()
    -- Get list of available cartridges from C++ API
    cartridges = list_cartridges()
    
    -- Debug: We can't use print() here because it requires x,y,color
    -- The cartridges list is ready now
end

function _update()
    local current_time = time()
    
    -- Navigate up (using btn with delay instead of btnp)
    if btn(2) and (current_time - last_input_time) > INPUT_DELAY then
        selected_index = selected_index - 1
        if selected_index < 1 then
            selected_index = #cartridges
        end
        
        -- Adjust scroll
        if selected_index < scroll_offset + 1 then
            scroll_offset = selected_index - 1
        end
        
        last_input_time = current_time
    end
    
    -- Navigate down
    if btn(3) and (current_time - last_input_time) > INPUT_DELAY then
        selected_index = selected_index + 1
        if selected_index > #cartridges then
            selected_index = 1
        end
        
        -- Adjust scroll
        if selected_index > scroll_offset + max_visible then
            scroll_offset = selected_index - max_visible
        end
        
        last_input_time = current_time
    end
    
    -- Load cartridge with Z or X button
    if (btn(4) or btn(5)) and (current_time - last_input_time) > INPUT_DELAY then
        if #cartridges > 0 then
            local cart = cartridges[selected_index]
            if cart.path and cart.path ~= "" then
                -- Phase 2.0.5.6: Open in Code Editor instead of running
                open_code_editor(cart.path)
            end
        end
        last_input_time = current_time
    end
end

function _draw()
    cls(COLOR_BG)
    
    -- Draw header
    print("ULICS FANTASY CONSOLE", 40, 8, COLOR_TITLE)
    print("v1.1.0 - PHOENIX RISING", 40, 16, COLOR_INFO)  -- Version info
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
            -- Draw selection bar that fits the text nicely
            -- Font is 4px wide per char, add space for "> " prefix and padding
            local text = "> " .. cart.name
            local text_width = #text * 4 + 8  -- 4px per char + 8px padding
            rectfill(6, y, text_width, 8, COLOR_SELECTED)  -- FIXED: (x,y,width,height)
            print(text, 8, y + 1, COLOR_BG)
        else
            print("  " .. cart.name, 8, y + 1, COLOR_TEXT)
        end
    end
    
    -- Draw selected cartridge info
    if selected_index > 0 and selected_index <= #cartridges then
        local cart = cartridges[selected_index]
        local info_y = 200
        
        line(0, info_y - 4, 255, info_y - 4, COLOR_TITLE)
        print("SELECTED:", 8, info_y, COLOR_TITLE)
        print(cart.name, 8, info_y + 8, COLOR_TEXT)
    end
    
    -- Draw controls (Phase 2.0.5.6: Updated to reflect Code Editor opening)
    print("UP/DOWN: SELECT  Z/X: OPEN EDITOR", 22, 240, COLOR_INFO)
end

-- Initialize on load
_init()
)";

} // namespace SystemScripts

#endif // SYSTEM_SCRIPTS_H
