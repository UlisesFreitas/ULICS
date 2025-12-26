-- Test Map API
-- Tests mget(), mset(), and map() functions

function _init()
    print("Map Test - Ready!")
end

function _update()
    --No updates needed
end

function _draw()
    clear(0) -- Clear to black

    -- Draw the map
    -- map(mx, my, sx, sy, w, h, [layer_mask])
    -- Screen is 128x64 pixels = 16x8 tiles
    -- Map is 128x64 tiles, so we draw the top-left 16x8 section
    map(0, 0, 0, 0, 16, 8)

    -- Draw some text over the map
    print("MAP TEST", 2, 2, 7)
    print("Use F3 to edit", 2, 10, 7)
end
