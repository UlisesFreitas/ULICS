-- ULICS Demo: PONG (2 Players)
-- Smooth version with collision fix

-- Player paddles
local p1_y = 112
local p2_y = 112

-- Ball
local ball_x = 128
local ball_y = 128
local ball_dx = 2
local ball_dy = 1
local ball_hit_cooldown = 0 -- Prevent multiple bounces

-- Scores
local score1 = 0
local score2 = 0

-- Game state
local winner = 0

function _init()
    -- Nothing needed
end

function _update()
    if winner > 0 then
        -- Game over, wait for restart
        if btnp(4) then -- Z to restart
            score1 = 0
            score2 = 0
            winner = 0
            ball_x = 128
            ball_y = 128
            ball_dx = 2
            ball_dy = 1
            ball_hit_cooldown = 0
        end
        return
    end

    -- Decrease cooldown
    if ball_hit_cooldown > 0 then
        ball_hit_cooldown = ball_hit_cooldown - 1
    end

    -- P1 controls (arrow keys)
    if btn(2) then p1_y = p1_y - 3 end -- UP
    if btn(3) then p1_y = p1_y + 3 end -- DOWN

    -- P2 controls (mouse)
    p2_y = mousey() - 16

    -- Keep paddles on screen
    if p1_y < 0 then p1_y = 0 end
    if p1_y > 224 then p1_y = 224 end
    if p2_y < 0 then p2_y = 0 end
    if p2_y > 224 then p2_y = 224 end

    -- Move ball
    ball_x = ball_x + ball_dx
    ball_y = ball_y + ball_dy

    -- Ball vs top/bottom
    if ball_y < 4 then
        ball_y = 4
        ball_dy = abs(ball_dy) -- Force down
    end
    if ball_y > 252 then
        ball_y = 252
        ball_dy = -abs(ball_dy) -- Force up
    end

    -- Ball vs left paddle (P1) - only if no cooldown and moving left
    if ball_dx < 0 and ball_hit_cooldown == 0 and ball_x < 16 then
        if ball_y > p1_y and ball_y < p1_y + 32 then
            ball_x = 16
            ball_dx = abs(ball_dx) -- Force right
            ball_hit_cooldown = 5  -- 5 frame cooldown
            -- Add spin
            local center = p1_y + 16
            ball_dy = (ball_y - center) / 8
        else
            -- P2 scores
            score2 = score2 + 1
            if score2 >= 5 then
                winner = 2
            end
            ball_x = 128
            ball_y = 128
            ball_dx = 2
            ball_dy = 1
            ball_hit_cooldown = 0
        end
    end

    -- Ball vs right paddle (P2) - only if no cooldown and moving right
    if ball_dx > 0 and ball_hit_cooldown == 0 and ball_x > 240 then
        if ball_y > p2_y and ball_y < p2_y + 32 then
            ball_x = 240
            ball_dx = -abs(ball_dx) -- Force left
            ball_hit_cooldown = 5   -- 5 frame cooldown
            -- Add spin
            local center = p2_y + 16
            ball_dy = (ball_y - center) / 8
        else
            -- P1 scores
            score1 = score1 + 1
            if score1 >= 5 then
                winner = 1
            end
            ball_x = 128
            ball_y = 128
            ball_dx = -2
            ball_dy = 1
            ball_hit_cooldown = 0
        end
    end
end

function _draw()
    cls(1)

    -- Center line
    for i = 0, 31 do
        if i % 2 == 0 then
            rectfill(127, i * 8, 2, 4, 6)
        end
    end

    -- Paddles
    rectfill(8, p1_y, 4, 32, 7)
    rectfill(244, p2_y, 4, 32, 7)

    -- Ball
    rectfill(ball_x - 2, ball_y - 2, 4, 4, 10)

    -- Scores
    print(score1, 100, 10, 7)
    print(score2, 148, 10, 7)

    -- Game over
    if winner > 0 then
        print("PLAYER " .. winner .. " WINS!", 80, 120, 11)
        print("PRESS Z TO RESTART", 70, 135, 7)
    end

    -- Instructions
    print("P1: ARROWS  P2: MOUSE", 60, 245, 6)
end
