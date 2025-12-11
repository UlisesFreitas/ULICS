-- ULICS Cartridge: Bouncing Balls
-- Demonstrates physics, particles, and higher performance

-- Ball physics system
local balls = {}
local num_balls = 50
local gravity = 0.2
local bounce_damping = 0.8

-- Initialize balls with random positions and velocities
function init_balls()
    for i = 1, num_balls do
        balls[i] = {
            x = rnd(256),
            y = rnd(100),
            vx = rnd(4) - 2,
            vy = 0,
            radius = 3 + rnd(5),
            color = flr(rnd(128)) -- Using 128-color palette
        }
    end
end

function _init()
    init_balls()
end

function _update()
    for i = 1, #balls do
        local b = balls[i]

        -- Apply gravity
        b.vy = b.vy + gravity

        -- Update position
        b.x = b.x + b.vx
        b.y = b.y + b.vy

        -- Bounce off walls
        if b.x - b.radius < 0 then
            b.x = b.radius
            b.vx = -b.vx * bounce_damping
        elseif b.x + b.radius > 256 then
            b.x = 256 - b.radius
            b.vx = -b.vx * bounce_damping
        end

        -- Bounce off floor
        if b.y + b.radius > 256 then
            b.y = 256 - b.radius
            b.vy = -b.vy * bounce_damping

            -- Stop small bounces
            if abs(b.vy) < 0.5 then
                b.vy = 0
            end
        end

        -- Bounce off ceiling
        if b.y - b.radius < 0 then
            b.y = b.radius
            b.vy = -b.vy * bounce_damping
        end

        -- Air resistance
        b.vx = b.vx * 0.995
    end
end

function _draw()
    cls(0) -- Black background

    -- Draw all balls
    for i = 1, #balls do
        local b = balls[i]
        circfill(b.x, b.y, b.radius, b.color)
    end

    -- Draw UI
    print("BOUNCING BALLS", 4, 4, 7)
    print("BALLS: " .. #balls, 4, 12, 6)
    print("128 COLORS!", 4, 20, 14)
    print("FPS: " .. flr(1 / 0.0166), 4, 28, 11)

    -- Draw floor line
    line(0, 255, 255, 255, 5)
end

-- Initialize on load
_init()
