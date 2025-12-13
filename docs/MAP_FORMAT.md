# ULICS Map Format & Layer System

**Version:** 1.1 (Updated for v1.0.0)  
**Last Updated:** 2025-12-13  
**Status:** Implemented & Functional

---

## Overview

ULICS maps are tile-based grids that reference sprites from the sprite sheet. Maps support **up to 8 layers** for:
- Parallax scrolling backgrounds
- Foreground/background separation
- Collision-specific layers
- Decorative overlays
- Multi-depth environments

---

## File Format

```
cartridges/my_game/
└── map.json       # Map data in JSON format
```

---

## Map Data Structure (map.json)

```json
{
  "version": "1.0",
  "width": 128,
  "height": 64,
  "tile_size": 8,
  "layers": [
    {
      "id": 0,
      "name": "far_background",
      "visible": true,
      "data": [0, 0, 1, 1, 2, 2, ...]
    },
    {
      "id": 1,
      "name": "platforms",
      "visible": true,
      "data": [0, 3, 3, 0, 0, 4, ...]
    },
    {
      "id": 2,
      "name": "decorations",
      "visible": true,
      "data": [...]
    }
  ]
}
```

### Fields:

- **version**: Format version ("1.0")
- **width**: Map width in tiles (max 128)
- **height**: Map height in tiles (max 64)
- **tile_size**: Size of each tile (typically 8x8)
- **layers**: Array of layer objects (up to 8)

### Layer Object:

- **id**: Layer index (0-7)
- **name**: Human-readable name (e.g., "background", "collision")
- **visible**: Whether layer is rendered by default
- **data**: Flat array of tile IDs (width × height integers)
  - Each value is a sprite ID (0-255)
  - Array is row-major: `data[y * width + x]`
  - **0 = transparent/empty** (allows layers below to show through)

---

## Lua API (v1.0.0 - Fully Functional)

### `map(mx, my, sx, sy, w, h, [layer_mask])`
Renders a portion of the map to the screen.

**Parameters:**
- `mx, my` - Map coordinates (in tiles) - where to read from
- `sx, sy` - Screen coordinates (in pixels) - where to draw
- `w, h` - Width/height to draw (in tiles)
- `layer_mask` - Layer bitfield (optional, default: 0xFF = all layers)

**Examples:**
```lua
-- Draw 16x16 tiles from map (0,0) to screen (0,0), all layers
map(0, 0, 0, 0, 16, 16)

-- Draw only layer 0 (background)
map(0, 0, 0, 0, 16, 16, 0x01)

-- Draw only layer 1 (foreground)
map(0, 0, 0, 0, 16, 16, 0x02)

-- Draw layers 0 AND 1 together
map(0, 0, 0, 0, 16, 16, 0x03)

-- Draw layers 0, 2, and 3 (skip layer 1)
map(0, 0, 0, 0, 16, 16, 0x0D)  -- Binary: 00001101
```

---

### `mget(x, y, [layer])`
Get tile ID at map coordinates.

**Parameters:**
- `x, y` - Map tile coordinates
- `layer` - Layer index 0-7 (optional, default: 0)

**Returns:** Sprite ID (0-255)

**Examples:**
```lua
-- Get tile from layer 0 (default)
local tile = mget(10, 5)

-- Get tile from layer 1 (collision layer)
local solid = mget(player.x / 8, player.y / 8, 1)
if solid > 0 then
    -- Collision detected
end

-- Check background decoration on layer 2
local deco = mget(5, 3, 2)
```

---

### `mset(x, y, tile_id, [layer])`
Set tile ID at map coordinates (dynamically modify map).

**Parameters:**
- `x, y` - Map tile coordinates
- `tile_id` - Sprite ID to set (0-255)
- `layer` - Layer index 0-7 (optional, default: 0)

**Examples:**
```lua
-- Clear tile on layer 0
mset(10, 5, 0)

-- Place platform tile on collision layer (1)
mset(15, 20, 5, 1)

-- Add decoration on foreground layer (2)
mset(8, 12, 42, 2)
```

---

## Layer Masking (Bitfield System)

Layers use **bitfield masking** for efficient selective rendering:

| Layer | Hex   | Binary    | Decimal |
|-------|-------|-----------|---------|
| 0     | 0x01  | 00000001  | 1       |
| 1     | 0x02  | 00000010  | 2       |
| 2     | 0x04  | 00000100  | 4       |
| 3     | 0x08  | 00001000  | 8       |
| 4     | 0x10  | 00010000  | 16      |
| 5     | 0x20  | 00100000  | 32      |
| 6     | 0x40  | 01000000  | 64      |
| 7     | 0x80  | 10000000  | 128     |
| All   | 0xFF  | 11111111  | 255     |

### Combining Layers:

```lua
-- Draw layers 0 and 1
local mask = 0x01 + 0x02  -- Or use: 0x03
map(0, 0, 0, 0, 16, 16, mask)

-- Draw layers 1, 2, and 4
local mask = 0x02 + 0x04 + 0x10  -- Or use: 0x16
map(0, 0, 0, 0, 16, 16, mask)
```

---

## Parallax Scrolling Tutorial

### Basic 2-Layer Parallax

```lua
-- Variables
local cam_x = 0
local bg_scroll = 0

function _update()
    -- Move camera
    if btn(1) then cam_x = cam_x + 2 end  -- Right
    if btn(0) then cam_x = cam_x - 2 end  -- Left
    
    -- Background moves slower (parallax effect)
    bg_scroll = cam_x * 0.5  -- 50% speed
end

function _draw()
    cls(12)  -- Sky blue
    
    -- Layer 0: Far background (slow movement)
    local bg_tile_x = flr(bg_scroll / 8)
    local bg_pixel_offset = -(bg_scroll % 8)
    map(bg_tile_x, 0, bg_pixel_offset, 0, 20, 16, 0x01)
    
    -- Layer 1: Foreground (normal speed)
    local fg_tile_x = flr(cam_x / 8)
    local fg_pixel_offset = -(cam_x % 8)
    map(fg_tile_x, 0, fg_pixel_offset, 0, 20, 16, 0x02)
    
    -- Player (always centered)
    spr(1, 128, 100)
end
```

### Multi-Layer Parallax (3+ Layers)

```lua
-- Parallax with 4 layers at different depths
local cam_x = 0

function _update()
    if btn(1) then cam_x = cam_x + 3 end
    if btn(0) then cam_x = cam_x - 3 end
end

function _draw()
    cls(12)
    
    -- Layer 0: Sky/clouds (20% speed - very slow)
    draw_parallax_layer(0, cam_x * 0.2, 0x01)
    
    -- Layer 1: Mountains (40% speed)
    draw_parallax_layer(1, cam_x * 0.4, 0x02)
    
    -- Layer 2: Trees (70% speed)
    draw_parallax_layer(2, cam_x * 0.7, 0x04)
    
    -- Layer 3: Ground (100% speed - normal)
    draw_parallax_layer(3, cam_x * 1.0, 0x08)
    
    -- Player
    spr(player_spr, 120, player_y)
end

function draw_parallax_layer(layer, scroll, mask)
    local tile_x = flr(scroll / 8)
    local pixel_x = -(scroll % 8)
    map(tile_x, 0, pixel_x, 0, 20, 16, mask)
end
```

---

## Layer Usage Patterns

### Pattern 1: Separation by Function

```lua
-- Layer 0: Visual background (non-interactive)
-- Layer 1: Collision/platforms (interactive)
-- Layer 2: Pickups/collectibles
-- Layer 3: Foreground decorations

function _draw()
    -- Draw all visual layers
    map(0, 0, 0, 0, 16, 16, 0x01)  -- Background
    map(0, 0, 0, 0, 16, 16, 0x02)  -- Platforms
    map(0, 0, 0, 0, 16, 16, 0x04)  -- Items
    
    -- Draw player
    spr(player, px, py)
    
    -- Draw foreground over player
    map(0, 0, 0, 0, 16, 16, 0x08)
end

function check_collision(x, y)
    -- Only check layer 1 (platforms)
    local tile = mget(flr(x/8), flr(y/8), 1)
    return tile > 0
end
```

### Pattern 2: Dynamic Layer Visibility

```lua
-- Toggle layers for debug/gameplay
local show_collision = false
local show_decorations = true

function _update()
    if btnp(10) then show_collision = not show_collision end  -- START
end

function _draw()
    local mask = 0x01  -- Always show background
    
    if show_decorations then
        mask = mask + 0x04  -- Add decorations layer
    end
    
    if show_collision then
        mask = mask + 0x02  -- Show collision layer
    end
    
    map(0, 0, 0, 0, 16, 16, mask)
end
```

### Pattern 3: Hidden Data Layers

```lua
-- Layer 0: Visual
-- Layer 1: Collision flags
-- Layer 2: Enemy spawn points (never rendered)
-- Layer 3: Item types (never rendered)

function _init()
    -- Read spawn data from hidden layer
    for y = 0, 15 do
        for x = 0, 15 do
            local enemy_type = mget(x, y, 2)
            if enemy_type > 0 then
                spawn_enemy(enemy_type, x * 8, y * 8)
            end
            
            local item = mget(x, y, 3)
            if item > 0 then
                spawn_item(item, x * 8, y * 8)
            end
        end
    end
    
    -- Clear hidden layers after reading
    -- (optional - keeps them as reference)
end

function _draw()
    -- Only draw layers 0 and 1 (visual + collision)
    map(0, 0, 0, 0, 16, 16, 0x03)
end
```

---

## Advanced Techniques

### Vertical Parallax

```lua
-- Parallax in both X and Y
function _draw()
    local scroll_x = cam_x * 0.3
    local scroll_y = cam_y * 0.3
    
    local tile_x = flr(scroll_x / 8)
    local tile_y = flr(scroll_y / 8)
    local px = -(scroll_x % 8)
    local py = -(scroll_y % 8)
    
    map(tile_x, tile_y, px, py, 20, 20, 0x01)
end
```

### Dynamic Tile Modification

```lua
-- Destroy tiles when hit
function break_tile(x, y)
    local tile_x = flr(x / 8)
    local tile_y = flr(y / 8)
    
    -- Check if breakable
    local tile = mget(tile_x, tile_y, 1)
    if tile == 5 then  -- Breakable block
        mset(tile_x, tile_y, 0, 1)  -- Remove it
        -- Spawn particles, sound, etc.
    end
end
```

### Layer-Based Collision

```lua
-- Different collision per layer
function get_collision_type(x, y)
    local tx = flr(x / 8)
    local ty = flr(y / 8)
    
    local solid = mget(tx, ty, 1)
    local hazard = mget(tx, ty, 2)
    
    if hazard > 0 then return "damage" end
    if solid > 0 then return "solid" end
    return " none"
end
```

---

## Performance Tips

1. **Layer Masking is Fast** - Draw only what you need
```lua
-- Good: Only 2 layers
map(0, 0, 0, 0, 16, 16, 0x03)

-- Avoid: Drawing all layers if not needed
map(0, 0, 0, 0, 16, 16, 0xFF)
```

2. **Tile 0 is Optimized** - Used to skip rendering
```lua
-- Efficient: Background with holes (tile 0 = transparent)
-- Foreground shows through automatically
```

3. **Culling is Automatic** - Off-screen tiles not drawn
```lua
-- Engine only renders tiles in viewport
-- No manual culling needed
```

---

## Example: Complete Platformer Map Setup

### map.json
```json
{
  "version": "1.0",
  "width": 128,
  "height": 64,
  "tile_size": 8,
  "layers": [
    {
      "id": 0,
      "name": "sky_clouds",
      "visible": true,
      "data": [/* Cloud tiles, mostly 0 */]
    },
    {
      "id": 1,
      "name": "platforms",
      "visible": true,
      "data": [/* Ground and platform tiles */]
    },
    {
      "id": 2,
      "name": "decorations",
      "visible": true,
      "data": [/* Grass, flowers, etc */]
    },
    {
      "id": 3,
      "name": "foreground",
      "visible": true,
      "data": [/* Trees, poles in front */]
    }
  ]
}
```

### main.lua
```lua
local cam_x = 0
local player = {x=64, y=200}

function _update()
    -- Player movement
    if btn(0) then player.x = player.x - 2; cam_x = cam_x - 2 end
    if btn(1) then player.x = player.x + 2; cam_x = cam_x + 2 end
    
    -- Collision check (layer 1 only)
    local solid = mget(flr(player.x/8), flr(player.y/8), 1)
end

function _draw()
    cls(12)
    
    -- Sky (20% parallax)
    draw_layer(0, cam_x * 0.2, 0x01)
    
    -- Platforms (100%)
    draw_layer(1, cam_x, 0x02)
    
    -- Decorations (100%)
    draw_layer(2, cam_x, 0x04)
    
    -- Player
    spr(1, 128, player.y)
    
    -- Foreground (100%)
    draw_layer(3, cam_x, 0x08)
end

function draw_layer(layer, scroll, mask)
    local tx = flr(scroll / 8)
    local px = -(scroll % 8)
    map(tx, 0, px, 0, 20, 32, mask)
end
```

---

## Limitations

- **Max Layers:** 8 (0-7)
- **Max Map Size:** 128x64 tiles (1024x512 pixels with 8x8 tiles)
- **Tile IDs:** 0-255 (256 unique tiles per layer)
- **Tile 0:** Always transparent

---

## Tools

**Current (v1.0.0):**
- Manual JSON editing
- External map editors (Tiled, etc.) with export

**Planned (v2.0):**
- Built-in Map Editor
- Visual layer management
- Real-time parallax preview

---

## See Also

- `LUA_API.md` - Complete API reference
- `SPRITE_FORMAT.md` - Sprite sheet format
- `CARTRIDGE_SPEC.md` - Cartridge structure

---

**Version:** 1.1  
**Last Updated:** 2025-12-13 (v1.0.0 release)  
**Status:** ✅ Fully Implemented & Functional
