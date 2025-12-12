# ULICS Map Format Specification

## Overview

ULICS maps are tile-based grids that reference sprites from the sprite sheet. Maps support **multiple layers** (up to 8) for parallax scrolling, foreground/background separation, and collision layers.

## File Format

```
cartridges/my_game/
└── map.json       # Map data in JSON format
```

## Map Data Structure (map.json)

```json
{
  "version": "1.0",
  "width": 32,
  "height": 32,
  "tile_size": 8,
  "layers": [
    {
      "id": 0,
      "name": "background",
      "visible": true,
      "data": [0, 0, 1, 1, 2, 2, ...]
    },
    {
      "id": 1,
      "name": "foreground",
      "visible": true,
      "data": [0, 3, 3, 0, 0, 4, ...]
    }
  ]
}
```

### Fields:

- **version**: Format version ("1.0")
- **width**: Map width in tiles (e.g., 32)
- **height**: Map height in tiles (e.g., 32)
- **tile_size**: Size of each tile (8 or 16, must match sprite sheet)
- **layers**: Array of layer objects

### Layer Object:

- **id**: Layer index (0-7)
- **name**: Human-readable name
- **visible**: Whether layer is rendered by default
- **data**: Flat array of tile IDs (width × height integers)
  - Each value is a sprite ID (0-255)
  - Array is row-major: `data[y * width + x]`
  - 0 typically means "empty" (transparent)

## Example: 4x4 Map

```json
{
  "version": "1.0",
  "width": 4,
  "height": 4,
  "tile_size": 8,
  "layers": [
    {
      "id": 0,
      "name": "ground",
      "visible": true,
      "data": [
        1, 1, 1, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 1, 1, 1
      ]
    }
  ]
}
```

## Lua API

### map(mx, my, sx, sy, w, h, [layer])
Renders a portion of the map.

- `mx, my`: Map coordinates (in tiles)
- `sx, sy`: Screen coordinates (in pixels)
- `w, h`: Width/height to draw (in tiles)
- `layer`: Layer mask (bitfield, default 0xFF = all layers)

```lua
-- Draw 16x16 tile region starting at map (0,0) to screen (0,0)
map(0, 0, 0, 0, 16, 16)

-- Draw only layer 0
map(0, 0, 0, 0, 16, 16, 0x01)

-- Draw layers 0 and 1
map(0, 0, 0, 0, 16, 16, 0x03)
```

### mget(x, y, [layer])
Get tile ID at map coordinates.

- `x, y`: Map tile coordinates
- `layer`: Layer index (default 0)
- Returns: Sprite ID (0-255)

```lua
local tile = mget(5, 10)  -- Get tile at (5,10) on layer 0
```

### mset(x, y, tile_id, [layer])
Set tile ID at map coordinates.

- `x, y`: Map tile coordinates
- `tile_id`: Sprite ID to set
- `layer`: Layer index (default 0)

```lua
mset(5, 10, 3)  -- Set tile (5,10) to sprite 3
```

## Layer Masking

Layers use **bitfield masking** for efficient rendering:

- Layer 0: `0x01` (binary: 00000001)
- Layer 1: `0x02` (binary: 00000010)
- Layer 2: `0x04` (binary: 00000100)
- ...
- All layers: `0xFF` (binary: 11111111)

```lua
-- Draw background only (layer 0)
map(0, 0, 0, 0, 32, 32, 0x01)

-- Draw foreground only (layer 1)
map(0, 0, 0, 0, 32, 32, 0x02)

-- Draw both background and foreground
map(0, 0, 0, 0, 32, 32, 0x03)
```

## Typical Layer Usage

- **Layer 0**: Background (sky, distant mountains)
- **Layer 1**: Ground/platforms (collision layer)
- **Layer 2**: Foreground decorations
- **Layer 3**: UI elements

## Implementation Notes

For Phase 5, maps will be loaded from JSON manually. In Phase 5.5, a visual Map Editor will be added.

---

**Version**: 1.0  
**Status**: Draft for Phase 5 Implementation  
**Author**: ULICS Development Team
