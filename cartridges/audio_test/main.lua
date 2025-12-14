-- Audio Test Cartridge
-- Tests sfx() and audio system

local test_phase = 1
local timer = 0

function _init()
    print("Audio Test Started!")
end

function _update()
    timer = timer + 1

    -- Test different sounds every 60 frames (1 second)
    if timer % 60 == 0 then
        test_phase = test_phase + 1
        if test_phase > 4 then
            test_phase = 1
        end
    end

    -- Play sound on Z press
    if btnp(4) then
        sfx(0) -- Play SFX 0
    end
end

function _draw()
    cls(1)

    print("AUDIO SYSTEM TEST", 70, 20, 7)
    print("=================", 70, 30, 6)

    print("Press Z to test sfx()", 60, 60, 11)

    print("Phase: " .. test_phase, 100, 90, 14)
    print("Timer: " .. timer, 100, 100, 6)

    -- Instructions
    print("This tests the lock-free", 55, 140, 7)
    print("ring buffer audio system", 55, 150, 7)
    print("(Bug 1.1.3 fix)", 80, 160, 10)

    print("If you hear sound without", 50, 190, 6)
    print("input lag, it works!", 60, 200, 11)

    print("ESC to exit", 90, 235, 8)
end
