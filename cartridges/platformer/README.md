# PLATFORMER - ULICS Demo

Simple platformer collecting coins.

## Controls

- **Arrow Keys** - Move left/right
- **Z / A Button** - Jump
- **Z (after winning)** - Restart

## How to Play

1. Collect all 5 yellow coins
2. Jump between platforms
3. Don't fall too far!

## Features Demonstrated

- `btn()` / `btnp()` - Button input (held vs pressed)
- `camera()` - Camera following player
- `circfill()` - Drawing circles
- `rectfill()` - Drawing platforms
- `print()` - UI text
- `sqrt()` - Distance calculation
- Physics (gravity, velocity, friction)
- Collision detection (AABB)
- Camera following
- Collectibles system

## Code Highlights

```lua
-- Gravity simulation
player.vy = player.vy + GRAVITY
if player.vy > MAX_FALL_SPEED then
    player.vy = MAX_FALL_SPEED
end

-- Platform collision
if player.vy > 0 and player.y < platform_y then
    player.y = platform_y - player.h
    player.vy = 0
    player.on_ground = true
end

-- Camera follow player
camera(player.x - 128, player.y - 128)
```

## Tips

- Hold jump longer for higher jumps
- Use momentum to reach distant platforms
- Coins respawn when you restart

Have fun platforming!
