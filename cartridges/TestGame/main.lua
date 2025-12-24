-- TestGame
-- Created with ULICS Cartridge Generator

-- Game state
player = {
    x = 120,
    y = 120,
    size = 8,
    color = 11
}

function _init()
    print("TestGame initialized!")
    print("Press arrow keys to move")
end

function _update()
    -- Movement
    if btn(0) then player.x = player.x - 2 end -- Left
    if btn(1) then player.x = player.x + 2 end -- Right
    if btn(2) then player.y = player.y - 2 end -- Up
    if btn(3) then player.y = player.y + 2 end -- Down

    -- Screen wrapping
    if player.x < 0 then player.x = 256 end
    if player.x > 256 then player.x = 0 end
    if player.y < 0 then player.y = 256 end
    if player.y > 256 then player.y = 0 end
end

function _draw()
    cls(0) -- Clear to dark blue

    spr(0, 10, 10)
    -- Draw player
    -- rectfill(player.x, player.y,
    --         player.x + player.size, player.y + player.size,
    --         player.color)

    -- Draw title
    print("TestGame", 80, 8, 7)

    -- Draw instructions
    print("Arrow Keys: Move", 8, 240, 6)
end
