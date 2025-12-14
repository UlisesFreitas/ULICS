-- Input Debug Test
-- Press ANY key to see if input is working

last_press = ""
press_count = 0

function _init()
    -- Ready
end

function _update()
    -- Test all buttons with btnp
    if btnp(0) then
        last_press = "LEFT"
        press_count = press_count + 1
    end
    if btnp(1) then
        last_press = "RIGHT"
        press_count = press_count + 1
    end
    if btnp(2) then
        last_press = "UP"
        press_count = press_count + 1
    end
    if btnp(3) then
        last_press = "DOWN"
        press_count = press_count + 1
    end
    if btnp(4) then
        last_press = "Z/BTN_A"
        press_count = press_count + 1
    end
    if btnp(5) then
        last_press = "X/BTN_B"
        press_count = press_count + 1
    end
end

function _draw()
    cls(1)

    print("INPUT DEBUG TEST", 60, 10, 7)
    print("Press any key", 70, 30, 6)

    -- Show continuous state with btn()
    local y = 60
    if btn(0) then
        print("LEFT DOWN", 80, y, 11)
        y = y + 10
    end
    if btn(1) then
        print("RIGHT DOWN", 80, y, 11)
        y = y + 10
    end
    if btn(2) then
        print("UP DOWN", 80, y, 11)
        y = y + 10
    end
    if btn(3) then
        print("DOWN DOWN", 80, y, 11)
        y = y + 10
    end
    if btn(4) then
        print("Z DOWN", 80, y, 11)
        y = y + 10
    end
    if btn(5) then
        print("X DOWN", 80, y, 11)
        y = y + 10
    end

    -- Show last btnp detection
    print("Last btnp(): " .. last_press, 40, 140, 14)
    print("Total presses: " .. press_count, 40, 160, 14)

    print("Watch btn() above & btnp() counter", 20, 200, 10)
end
