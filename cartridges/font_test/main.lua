-- Font rendering test
-- Test how text appears on screen

function _init()
end

function _update()
end

function _draw()
    cls(0) -- Black

    -- Test different text at different positions
    print("local x = 10", 10, 10, 7)
    print("function test()", 10, 20, 7)
    print("abcdefghijklmnopqrstuvwxyz", 10, 30, 7)
    print("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 10, 40, 7)
    print("0123456789", 10, 50, 7)
    print("!@#$%^&*()", 10, 60, 7)

    -- Test at exact positions from editor
    print("local x = 10", 44, 12, 7) -- Same as editor TEXT_X, first line
    print("y = 20", 44, 23, 7)       -- Second line (12 + 11)
    print("print(x, y)", 44, 34, 7)  -- Third line
end
