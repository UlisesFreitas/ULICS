# ULICS Cartridges

This directory contains ULICS cartridges - self-contained games and demos.

## Structure

A ULICS cartridge can be:

1. **Directory-based** (recommended):
   ```
   cartridges/
   └── my_game/
       ├── main.lua       # REQUIRED: Game code
       └── config.json    # OPTIONAL: Configuration
   ```

2. **Single file**:
   ```
   cartridges/
   └── simple_game.lua
   ```

## Configuration (config.json)

Optional but recommended. Example:

```json
{
  "name": "My Game",
  "author": "Your Name",
  "version": "1.0.0",
  "description": "A cool game",
  
  "memory_limit_mb": 768,
  "lua_code_limit_lines": 500000,
  
  "palette_size": 64,
  "framebuffer_width": 256,
  "framebuffer_height": 256,
  
  "target_fps": 60
}
```

### Resource Limits (ULICS Differentiator)

- **memory_limit_mb**: 512-1024 MB (vs 2MB in PICO-8) ⚡
- **lua_code_limit_lines**: 1-1,000,000 lines (vs 8K tokens in PICO-8) ⚡

These generous limits enable:
- Complex RPGs with thousands of quests
- Procedural worlds (10,000 x 10,000 tiles)
- Advanced AI (behavior trees, pathfinding)
- Massive sprite/asset libraries

## Running Cartridges

### From Command Line

```bash
# Run specific cartridge
UliCS.exe cartridges/my_game

# Run single .lua file
UliCS.exe cartridges/test.lua
```

### From Menu (Future)

Just run `UliCS.exe` without arguments to see the cartridge menu.

## Included Demos

### hello_world
- **Purpose**: Minimal example showing basic API
- **Palette**: 16 colors (classic)
- **Features**: Text rendering, color cycling, time display

### bouncing_ball
- **Purpose**: Physics and performance demo
- **Palette**: 128 colors (high-performance mode)
- **Features**: 50 bouncing balls, gravity, collision detection
- **Showcases**: ULICS's ability to handle complex simulations

## Creating Your Own Cartridge

### Minimal Example

`cartridges/test/main.lua`:
```lua
function _update()
    -- Game logic (runs 60 times per second)
end

function _draw()
    cls(1)  -- Clear to dark blue
    print("HELLO!", 100, 120, 7)
end
```

### With Physics

See `bouncing_ball/main.lua` for a full example with:
- Particle system
- Physics simulation
- Multiple entities

### Best Practices

1. **Use config.json** to specify your needs
2. **Request what you need**: Don't over-allocate memory
3. **Test performance**: Use `time()` to measure
4. **Code organization**: Use Lua modules for large projects

## ULICS API Reference

See `LUA_API.md` in the root directory for complete API documentation.

Key functions:
- `cls(color)` / `clear(color)` - Clear screen
- `pset(x, y, color)` - Draw pixel
- `print(text, x, y, color)` - Draw text
- `line()`, `rect()`, `circ()` - Shape primitives
- `btn(i)`, `btnp(i)` - Input
- `time()` - Get elapsed time
- Math: `sin()`, `cos()`, `rnd()`, `flr()`, etc.

## Philosophy

ULICS cartridges follow the "everything is code" philosophy:
- No external image files
- No external sound files
- All assets defined as Lua data structures

But with ULICS's massive resources (512MB-1GB RAM, 1M lines), you can have:
- Thousands of procedurally generated sprites
- Complex audio synthesis
- Massive procedural worlds
- Advanced AI systems

**This is what differentiates ULICS from PICO-8/TIC-80!**
