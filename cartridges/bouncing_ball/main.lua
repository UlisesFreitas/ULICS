-- ULICS Cartridge: Static Test
-- No updates, just static drawing

function _draw()
    cls(1) -- Dark blue background

    -- Draw static circles in fixed positions
    circ(50, 50, 20, 8)
    circ(130, 50, 20, 9)
    circ(200, 50, 20, 10)

    circ(50, 130, 20, 11)
    circ(130, 130, 20, 12)
    circ(200, 130, 20, 13)

    circ(50, 200, 20, 14)
    circ(130, 200, 20, 15)
    circ(200, 200, 20, 7)

    -- Text
    print("STATIC CIRCLES TEST", 50, 10, 7)
    print("NO ANIMATION", 80, 230, 6)
end

function _update()
    -- Do nothing - no updates
end
