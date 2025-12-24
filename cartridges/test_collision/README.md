# Sprite Flags Collision Test

Demo cartridge that demonstrates **sprite flag-based collision detection** in ULICS.

## What This Demonstrates

- **Sprite 0 (Player)** has **FLAG 0** set in `spritesheet.flags`
- **Sprite 1 (Platform)** has **FLAG 1** set in `spritesheet.flags`
- Collision detection uses `fget(sprite_id, flag_bit)` to check if a sprite is solid
- Fully functional platformer with physics

## Controls

- **Arrow LEFT/RIGHT**: Move player
- **Z**: Jump (only when on ground)

## How It Works

### Sprite Flags

Flags are stored in `spritesheet.flags` (256 bytes, 1 byte per sprite):
- Byte 0: `0x01` (binary `00000001`) → Sprite 0 has FLAG 0
- Byte 1: `0x02` (binary `00000010`) → Sprite 1 has FLAG 1

### Collision Detection

```lua
-- Check if sprite 1 has FLAG 1 (solid)
if fget(1, 1) then
    -- Perform AABB collision with platform
    if player_overlaps_platform then
        -- Handle collision (land on top, hit ceiling, etc)
    end
end
```

## API Used

### `fget(sprite_id, [flag_bit])`
- `fget(n)` - Returns all 8 flags as byte (0-255)
- `fget(n, bit)` - Returns true/false for specific flag (bit 0-7)

### `fset(sprite_id, [flag_bit], [value])`
- `fset(n, flags)` - Set all 8 flags from byte
- `fset(n, bit, value)` - Set specific flag to true/false

## Files

- `main.lua` - Game logic with flag-based collision
- `spritesheet.png` - 128x128 sprite sheet (player + platform sprites)
- `spritesheet.flags` - Binary file with flag data (256 bytes)
- `config.json` - Cartridge metadata

## Important Notes

### Workaround: Variable Initialization

Due to how ULICS currently handles `_init()`, global data must be initialized **at module level**:

```lua
-- ✅ WORKS - Initialize at module level
platforms = {
    {x = 40, y = 200, w = 6},
    {x = 100, y = 160, w = 5}
}

function _init()
    -- Leave empty or use for other initialization
end

-- ❌ DOESN'T WORK - Initialize in _init()
platforms = {}

function _init()
    platforms[1] = {x = 40, y = 200, w = 6}  -- Gets reset!
end
```

This will be addressed when the Map Editor is implemented (platforms will come from map data).

## Future Enhancements

- **Map Editor Integration**: Load platforms from map tiles
- **More Flags**: FLAG 2 (hazards), FLAG 3 (collectibles), FLAG 4 (one-way platforms)
- **Dynamic Flag Modification**: Use `fset()` to change sprite flags at runtime

## Technical Details

Sprite flags are loaded by the Engine when a cartridge is loaded:
- `Engine.cpp` calls `SpriteEditor::LoadSpriteFlags()` 
- Flags are read from `spritesheet.flags` binary file
- `fget()`/`fset()` are available in Lua via `ScriptingManager`

## References

- Main code: `cartridges/test_collision/main.lua`
- Lua API: `docs/app/LUA_API.md`
- Engine code: `src/core/Engine.cpp` (flag loading)
- Scripting bindings: `src/scripting/ScriptingManager.cpp`
