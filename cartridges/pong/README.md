# PONG - ULICS Demo

Classic 2-player Pong game.

## Controls

**Player 1:**
- UP/DOWN Arrow Keys - Move paddle

**Player 2:**
- Mouse Y Position - Move paddle

**Both Players:**
- Z / A Button - Restart game (after game over)

## How to Play

1. First player to score 5 points wins
2. Ball speeds up with each hit
3. Hit the ball with different parts of the paddle for spin

## Features Demonstrated

- `btn()` - Button input
- `mousey()` - Mouse input
- `rectfill()` - Drawing rectangles
- `print()` - Text rendering
- Physics simulation
- Game state management
- Score tracking

## Code Highlights

```lua
-- Paddle collision with spin
local hit_pos = (ball.y - p1.y) / p1.h  -- 0-1
ball.vy = (hit_pos - 0.5) * 4  -- Add spin

-- Ball speed increase
ball.speed_mult = ball.speed_mult + 0.05
```

Enjoy the classic game!
