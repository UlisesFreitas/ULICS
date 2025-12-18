# ULICS Sprite Format Specification

## Overview

ULICS sprites are stored in a **PNG spritesheet** combined with **JSON metadata**. This approach allows:
- Visual editing in external tools (Aseprite, Photoshop, etc.)
- Indexed color workflow (palette-based)
- Easy import/export for artists

## File Structure

```
cartridges/my_game/
├── sprites.png       # 128x128 pixel spritesheet
└── sprites.json      # Metadata (optional)
```

## Spritesheet Format (sprites.png)

### Specifications:
- **Size**: 128x128 pixels (fixed)
- **Format**: PNG, indexed color (palette mode) or RGB
- **Color Depth**: 8-bit (256 colors max, but ULICS palette is 16-256)
- **Grid**: 8x8 pixels per sprite = 16x16 grid of sprites (256 sprites total)
- **Alternative**: 16x16 pixels per sprite = 8x8 grid (64 sprites total)

### Layout:
```
+---+---+---+---+---+---+---+---+
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |  <- Sprite IDs (8x8 tiles)
+---+---+---+---+---+---+---+---+
| 8 | 9 |10 |11 |12 |13 |14 |15 |
+---+---+---+---+---+---+---+---+
|16 |17 |18 |19 |20 |21 |22 |23 |
...
```

### Transparency:
- **Color Index 0** is transparent by default
- Can be changed with `tcolor(index)` in Lua

## Metadata Format (sprites.json)

Optional JSON file for sprite metadata:

```json
{
  "version": "1.0",
  "tile_size": 8,
  "total_sprites": 256,
  "sprites": [
    {
      "id": 0,
      "name": "empty",
      "tags": ["background"]
    },
    {
      "id": 1,
      "name": "player_idle",
      "tags": ["player", "animated"],
      "animation": {
        "frames": [1, 2, 3, 4],
        "speed": 4
      }
    }
  ]
}
```

### Fields:
- `version`: Format version (currently "1.0")
- `tile_size`: 8 or 16 pixels
- `total_sprites`: Calculated from layout
- `sprites`: Array of sprite metadata (optional)
  - `id`: Sprite index (0-255)
  - `name`: Human-readable name
  - `tags`: Array of tags for organization
  - `animation`: Animation data (frame IDs, speed)

## Color Mapping

ULICS uses **palette indices** internally:

1. **If PNG is indexed color**: Pixel values map directly to ULICS palette indices
2. **If PNG is RGB**: 
   - Find nearest color in active ULICS palette
   - Convert to palette index at load time

## Sprite IDs

Sprites are referenced by **ID (0-255)** in both:
- Lua API: `spr(1, x, y)` draws sprite #1
- Map data: Each tile stores a sprite ID

## Example Workflow

### For Developers (Phase 5):
1. Create `sprites.png` manually in any image editor
2. Use indexed color mode with PICO-8 or custom palette
3. Save as PNG
4. Optionally create `sprites.json` for metadata
5. Place both in cartridge directory

### For Users (Phase 5.5):
1. Press F1 in ULICS to open Sprite Editor
2. Click pixels to draw
3. Choose colors from palette
4. Auto-saves to `sprites.png`

## Implementation Notes

### Loading (C++):
```cpp
// SpriteSheet.cpp
bool SpriteSheet::LoadFromPNG(const std::string& path) {
    // Use stb_image to load PNG
    // Convert RGB to palette indices if needed
    // Store in 128x128 array of uint8_t (palette indices)
}
```

### Rendering (C++):
```cpp
// AestheticLayer.cpp
void DrawSprite(int id, int x, int y, int w, int h) {
    // Calculate sprite position in 128x128 sheet
    // Copy pixels to framebuffer
    // Respect tcolor for transparency
}
```

### Lua API:
```lua
-- Basic sprite draw
spr(1, 64, 64)        -- Draw sprite 1 at (64, 64)

-- With size (1x1, 2x2, etc.)
spr(1, 64, 64, 2, 2)  -- Draw 2x2 sprite grid starting at sprite 1

-- Flipped
spr(1, 64, 64, 1, 1, true, false)  -- Flip horizontally

-- Advanced: sspr (sprite sheet sprite)
sspr(8, 8, 16, 16, 64, 64, 32, 32)  -- Copy 16x16 region, scale to 32x32
```

## Future Extensions

- **Multiple spritesheets**: Load multiple 128x128 sheets
- **Different tile sizes**: Mix 8x8 and 16x16 in same sheet
- **Compression**: Optional PNG compression for smaller files
- **Runtime editing**: Modify sprites from Lua code (pixel-art generation)

---

**Version**: 1.0  
**Status**: Draft for Phase 5 Implementation  
**Author**: ULICS Development Team
