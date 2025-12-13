-- ULICS Basic SFX Demo
-- Demonstrates audio API with simple sound effects
-- Press Z for beep, X for boop

local sfx_played = {
    beep = false,
    boop = false
}

local flash_timer = {
    beep = 0,
    boop = 0
}

function _init()
    cls(0)
    print("ULICS AUDIO DEMO", 60, 20, 7)
    print("Press Z for BEEP", 50, 80, 11)
    print("Press X for BOOP", 50, 100, 12)
end

function _update()
    -- Check for Z key (button 4)
    if btnp(4) then
        sfx(0) -- Play beep sound
        flash_timer.beep = 10
        sfx_played.beep = true
    end

    -- Check for X key (button 5)
    if btnp(5) then
        sfx(1) -- Play boop sound
        flash_timer.boop = 10
        sfx_played.boop = true
    end

    -- Decrease flash timers
    if flash_timer.beep > 0 then
        flash_timer.beep = flash_timer.beep - 1
    end
    if flash_timer.boop > 0 then
        flash_timer.boop = flash_timer.boop - 1
    end
end

function _draw()
    cls(0)

    -- Title
    print("ULICS AUDIO DEMO", 60, 20, 7)
    line(40, 35, 215, 35, 7)

    -- Instructions
    print("Press Z for BEEP", 50, 60, 11)
    print("Press X for BOOP", 50, 80, 12)

    -- Visual feedback for beep (Z)
    if flash_timer.beep > 0 then
        circfill(30, 65, 8, 11)
        print("BEEP!", 170, 60, 11)
    else
        circ(30, 65, 8, 5)
    end

    -- Visual feedback for boop (X)
    if flash_timer.boop > 0 then
        circfill(30, 85, 8, 12)
        print("BOOP!", 170, 80, 12)
    else
        circ(30, 85, 8, 5)
    end

    -- Stats
    print("SFX Played:", 50, 120, 6)
    if sfx_played.beep then
        print("Beep: YES", 60, 135, 11)
    else
        print("Beep: NO", 60, 135, 5)
    end

    if sfx_played.boop then
        print("Boop: YES", 60, 150, 12)
    else
        print("Boop: NO", 60, 150, 5)
    end

    -- Footer
    line(20, 200, 235, 200, 7)
    print("ULICS Audio System (SDL2)", 45, 210, 14)
    print("Z=Button4  X=Button5", 55, 230, 6)
end
