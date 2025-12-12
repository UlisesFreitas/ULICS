-- ULICS: Bouncing Balls Demo
-- Simple physics demonstration

local balls = {}
local num_balls = 8

function _init()
    -- Create balls with random initial positions
    for i = 1, num_balls do
        balls[i] = {
            x = 30 + (i - 1) * 25,
            y = 50 + rnd(50),
            vx = rnd(2) - 1,
            vy = 0,
            r = 6,
            c = 7 + (i % 9) -- Colors 7-15
        }
    end
end

function _update()
    for i = 1, #balls do
        local b = balls[i]

        -- Gravity
        b.vy = b.vy + 0.3

        -- Update position
        b.x = b.x + b.vx
        b.y = b.y + b.vy

        -- Bounce off left/right walls
        if b.x < b.r then
            b.x = b.r
            b.vx = -b.vx * 0.8
        elseif b.x > 256 - b.r then
            b.x = 256 - b.r
            b.vx = -b.vx * 0.8
        end

        -- Bounce off floor
        if b.y > 256 - b.r then
            b.y = 256 - b.r
            b.vy = -b.vy * 0.75

            -- Stop tiny bounces
            if abs(b.vy) < 0.5 then
                b.vy = 0
            end
        end

        -- Bounce off ceiling
        if b.y < b.r then
            b.y = b.r
            b.vy = abs(b.vy) * 0.5
        end

        -- Friction
        b.vx = b.vx * 0.99
    end
end

function _draw()
    cls(0) -- Black background

    -- Title
    print("BOUNCING BALLS", 70, 5, 7)
    print("PHYSICS DEMO", 75, 15, 6)

    -- Draw balls
    for i = 1, #balls do
        local b = balls[i]
        -- Draw filled circle (multiple concentric circles)
        for r = b.r, 0, -1 do
            circ(b.x, b.y, r, b.c)
        end
    end

    -- Footer
    print("ESC: MENU", 5, 245, 14)

    -- Draw floor indicator
    line(0, 256 - 6, 255, 256 - 6, 5)
end

_init()
