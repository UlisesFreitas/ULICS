# ULICS Lua API Reference

Complete reference for all 40 Lua functions available in ULICS fantasy console.

**Version:** 1.0  
**Total Functions:** 40  
**Category Breakdown:** Graphics (11), Input (5), Sprites (2), Maps (3), Audio (2), Math (8), System (6), Helpers (3)

---

## Table of Contents

- [Graphics API](#graphics-api) - Drawing primitives, colors, camera
- [Input API](#input-api) - Keyboard, mouse, gamepad
- [Sprite API](#sprite-api) - Sprite rendering
- [Map API](#map-api) - Tilemap rendering
- [Audio API](#audio-api) - Sound effects and music
- [Math API](#math-api) - Mathematical functions
- [System API](#system-api) - Time, cartridge management
- [Helper Functions](#helper-functions) - Utilities

---

## Graphics API

### `cls([color])`
Clear the screen with a color.

**Parameters:**
- `color` (optional): Color index (0-15). Default: 0 (black)

**Example:**
```lua
function _draw()
    cls(1)  -- Clear to dark blue
end
```

---

### `clear([color])`
Alias for `cls()`. Clears the screen.

**Parameters:**
- `color` (optional): Color index (0-15). Default: 0

**Example:**
```lua
clear(5)  -- Clear to dark gray
```

---

### `pset(x, y, [color])`
Set a pixel at coordinates.

**Parameters:**
- `x`: X coordinate (0-255)
- `y`: Y coordinate (0-255)
- `color` (optional): Color index (0-15). Default: 7 (white)

**Example:**
```lua
pset(128, 128, 8)  -- Draw red pixel at center
```

---

### `pget(x, y)`
Get the color of a pixel.

**Parameters:**
- `x`: X coordinate
- `y`: Y coordinate

**Returns:** Color index (0-15)

**Example:**
```lua
local c = pget(10, 20)
if c == 8 then
    print("Red pixel!")
end
```

---

### `line(x1, y1, x2, y2, [color])`
Draw a line between two points.

**Parameters:**
- `x1, y1`: Start point
- `x2, y2`: End point
- `color` (optional): Color index. Default: 7

**Example:**
```lua
line(0, 0, 255, 255, 11)  -- Diagonal green line
```

---

### `rect(x, y, w, h, [color])`
Draw a rectangle outline.

**Parameters:**
- `x, y`: Top-left corner
- `w, h`: Width and height
- `color` (optional): Color index. Default: 7

**Example:**
```lua
rect(10, 10, 50, 30, 12)  -- Light blue rectangle
```

---

### `rectfill(x, y, w, h, [color])`
Draw a filled rectangle.

**Parameters:**
- `x, y`: Top-left corner
- `w, h`: Width and height
- `color` (optional): Color index. Default: 7

**Example:**
```lua
rectfill(20, 20, 100, 60, 9)  -- Orange filled box
```

---

### `circ(x, y, r, [color])`
Draw a circle outline.

**Parameters:**
- `x, y`: Center point
- `r`: Radius
- `color` (optional): Color index. Default: 7

**Example:**
```lua
circ(128, 128, 50, 10)  -- Yellow circle
```

---

### `circfill(x, y, r, [color])`
Draw a filled circle.

**Parameters:**
- `x, y`: Center point
- `r`: Radius
- `color` (optional): Color index. Default: 7

**Example:**
```lua
circfill(64, 64, 20, 14)  -- Pink filled circle
```

---

### `print(text, x, y, [color])`
Draw text on screen.

**Parameters:**
- `text`: String to print
- `x, y`: Position
- `color` (optional): Color index. Default: 7

**Returns:** X coordinate after text (for chaining)

**Example:**
```lua
print("SCORE: 100", 10, 10, 11)
local next_x = print("LIVES:", 10, 20, 8)
print(player.lives, next_x + 2, 20, 8)
```

---

### `camera([x], [y])`
Set camera offset for drawing.

**Parameters:**
- `x` (optional): X offset. Default: 0
- `y` (optional): Y offset. Default: 0

**Notes:** All drawing functions are offset by camera values. Call `camera()` with no args to reset.

**Example:**
```lua
-- Follow player
camera(player.x - 128, player.y - 128)

-- Reset camera
camera()
```

---

### `tcolor([index])`
Set transparent color for sprites.

**Parameters:**
- `index` (optional): Color to treat as transparent. Default: 0 (black)

**Example:**
```lua
tcolor(5)  -- Make dark gray transparent
spr(0, 64, 64)  -- Sprite drawn with transparency
```

---

## Input API

### `btn(button_id)`
Check if a button is held down.

**Parameters:**
- `button_id`: Button index (0-11)
  - 0: LEFT, 1: RIGHT, 2: UP, 3: DOWN
  - 4: A, 5: B, 6: X, 7: Y
  - 8: L, 9: R, 10: START, 11: SELECT

**Returns:** `true` if button is held, `false` otherwise

**Notes:** Works with both keyboard and gamepad automatically.

**Example:**
```lua
if btn(0) then
    player.x = player.x - 2  -- Move left while held
end
```

---

### `btnp(button_id)`
Check if a button was just pressed (one frame).

**Parameters:**
- `button_id`: Button index (0-11)

**Returns:** `true` if button was just pressed, `false` otherwise

**Example:**
```lua
if btnp(4) then
    player_jump()  -- Jump on A press
end
```

---

### `mouse()`
Get complete mouse state.

**Returns:** Table with:
- `x`: Mouse X position
- `y`: Mouse Y position
- `left`: Left button state (boolean)
- `right`: Right button state (boolean)
- `middle`: Middle button state (boolean)

**Example:**
```lua
local m = mouse()
if m.left then
    shoot_at(m.x, m.y)
end
```

---

### `mousex()`
Get mouse X coordinate.

**Returns:** X position (0-255)

**Example:**
```lua
local mx = mousex()
cursor.x = mx
```

---

### `mousey()`
Get mouse Y coordinate.

**Returns:** Y position (0-255)

**Example:**
```lua
local my = mousey()
cursor.y = my
```

---

## Sprite API

### `spr(sprite_id, x, y, [w], [h], [flip_x], [flip_y])`
Draw a sprite.

**Parameters:**
- `sprite_id`: Sprite index from spritesheet
- `x, y`: Position to draw
- `w` (optional): Width in sprites. Default: 1
- `h` (optional): Height in sprites. Default: 1
- `flip_x` (optional): Flip horizontally. Default: false
- `flip_y` (optional): Flip vertically. Default: false

**Example:**
```lua
spr(0, 64, 64)              -- Draw sprite 0
spr(1, 100, 100, 2, 2)      -- Draw 2x2 sprite
spr(2, 50, 50, 1, 1, true)  -- Draw flipped sprite
```

---

### `sspr(sx, sy, sw, sh, dx, dy, [dw], [dh])`
Draw a section of the spritesheet.

**Parameters:**
- `sx, sy`: Source position in spritesheet
- `sw, sh`: Source width/height
- `dx, dy`: Destination position
- `dw, dh` (optional): Destination size (for scaling)

**Example:**
```lua
-- Draw 16x16 section from spritesheet
sspr(0, 0, 16, 16, 64, 64)

-- Draw and scale to 32x32
sspr(0, 0, 16, 16, 64, 64, 32, 32)
```

---

## Map API

### `map(cell_x, cell_y, screen_x, screen_y, cells_w, cells_h, [layer])`
Draw a section of the map.

**Parameters:**
- `cell_x, cell_y`: Map cell coordinates to start from
- `screen_x, screen_y`: Screen position to draw at
- `cells_w, cells_h`: Number of cells to draw
- `layer` (optional): Layer mask (bit flags). Default: 0xFF (all layers)

**Example:**
```lua
-- Draw 16x16 cells from map position 0,0 at screen 0,0
map(0, 0, 0, 0, 16, 16)

-- Draw only layer 0
map(0, 0, 0, 0, 16, 16, 0x01)
```

---

### `mget(x, y, [layer])`
Get tile ID at map coordinates.

**Parameters:**
- `x, y`: Map cell coordinates
- `layer` (optional): Layer number. Default: 0

**Returns:** Tile ID (0-255)

**Example:**
```lua
local tile = mget(player.mx, player.my)
if tile == 1 then
    -- Collision with wall
end
```

---

### `mset(x, y, tile_id, [layer])`
Set tile ID at map coordinates.

**Parameters:**
- `x, y`: Map cell coordinates
- `tile_id`: Tile ID to set (0-255)
- `layer` (optional): Layer number. Default: 0

**Example:**
```lua
mset(10, 5, 0)  -- Clear tile
mset(10, 6, 15) -- Place tile 15
```

---

## Audio API

### `sfx(id, [channel], [offset])`
Play a sound effect.

**Parameters:**
- `id`: SFX ID (0-255), or -1 to stop
- `channel` (optional): Channel (0-7), or -1 for auto. Default: -1
- `offset` (optional): Time offset in seconds. Default: 0

**Notes:** Audio synthesis not yet implemented (Phase 6).

**Example:**
```lua
sfx(0)      -- Play SFX 0 on auto channel
sfx(1, 2)   -- Play SFX 1 on channel 2
sfx(-1)     -- Stop all SFX
```

---

### `music(pattern_id, [fade_ms], [channel_mask])`
Play or stop music.

**Parameters:**
- `pattern_id`: Pattern ID (0-255), or -1 to stop
- `fade_ms` (optional): Fade duration in milliseconds. Default: 0
- `channel_mask` (optional): Channels to play (bit flags). Default: 0x0F (all 4)

**Example:**
```lua
music(0)          -- Play pattern 0
music(0, 500)     -- Fade in over 500ms
music(-1, 1000)   -- Fade out over 1 second
```

---

## Math API

### `sin(x)`
Sine function (0.0 to 1.0 range).

**Parameters:**
- `x`: Angle from 0.0 to 1.0 (not radians!)

**Returns:** Sine value (-1.0 to 1.0)

**Notes:** ULICS uses 0-1 range for angles, not 0-2π

**Example:**
```lua
local s = sin(0.25)  -- sin(90 degrees) = 1.0
```

---

### `cos(x)`
Cosine function (0.0 to 1.0 range).

**Parameters:**
- `x`: Angle from 0.0 to 1.0

**Returns:** Cosine value (-1.0 to 1.0)

**Example:**
```lua
local c = cos(0)  -- cos(0 degrees) = 1.0
```

---

### `atan2(dx, dy)`
Arctangent of dy/dx.

**Parameters:**
- `dx`: Delta X
- `dy`: Delta Y

**Returns:** Angle from 0.0 to 1.0

**Example:**
```lua
local angle = atan2(enemy.x - player.x, enemy.y - player.y)
```

---

### `sqrt(x)`
Square root.

**Parameters:**
- `x`: Number

**Returns:** Square root of x

**Example:**
```lua
local dist = sqrt(dx*dx + dy*dy)
```

---

### `abs(x)`
Absolute value.

**Parameters:**
- `x`: Number

**Returns:** Absolute value

**Example:**
```lua
local speed = abs(velocity)
```

---

### `flr(x)`
Floor (round down).

**Parameters:**
- `x`: Number

**Returns:** Largest integer ≤ x

**Example:**
```lua
local tile_x = flr(player.x / 8)
```

---

### `ceil(x)`
Ceiling (round up).

**Parameters:**
- `x`: Number

**Returns:** Smallest integer ≥ x

**Example:**
```lua
local needed = ceil(health / 10)
```

---

### `rnd([max])`
Random number.

**Parameters:**
- `max` (optional): Maximum value. Default: 1.0

**Returns:** Random number from 0 to max (exclusive)

**Example:**
```lua
local roll = flr(rnd(6)) + 1  -- Dice roll (1-6)
local chance = rnd(1)         -- Random 0.0-1.0
```

---

## System API

### `time()`
Get elapsed time since cartridge start.

**Returns:** Time in seconds (floating point)

**Example:**
```lua
local t = time()
local angle = sin(t * 0.1)  -- Oscillate over time
```

---

### `load_cartridge(name)`
Load and run a different cartridge.

**Parameters:**
- `name`: Cartridge folder name

**Example:**
```lua
if btnp(10) then
    load_cartridge("menu")
end
```

---

### `list_cartridges()`
Get list of available cartridges.

**Returns:** Table of cartridge names

**Example:**
```lua
local carts = list_cartridges()
for i, name in ipairs(carts) do
    print(name, 10, i * 10, 7)
end
```

---

### `exit()`
Quit ULICS.

**Example:**
```lua
if btnp(11) then
    exit()
end
```

---

### `reset()`
Restart current cartridge.

**Example:**
```lua
if game_over then
    reset()
end
```

---

### `goto_menu()`
Return to cartridge selection menu.

**Example:**
```lua
if btnp(10) then
    goto_menu()
end
```

---

## Helper Functions

### `_init()`
Called once when cartridge starts.

**Notes:** Define this function in your cartridge.

**Example:**
```lua
function _init()
    player = {x=128, y=128, speed=2}
    score = 0
end
```

---

### `_update()`
Called every frame (60 FPS) for game logic.

**Notes:** Define this function in your cartridge.

**Example:**
```lua
function _update()
    if btn(0) then player.x = player.x - player.speed end
    if btn(1) then player.x = player.x + player.speed end
end
```

---

### `_draw()`
Called every frame for rendering.

**Notes:** Define this function in your cartridge.

**Example:**
```lua
function _draw()
    cls(1)
    circfill(player.x, player.y, 4, 8)
    print("SCORE: " .. score, 10, 10, 7)
end
```

---

## Color Palette

ULICS uses a 16-color palette (indices 0-15):

| Index | Color Name    | RGB (approx) |
|-------|---------------|--------------|
| 0     | Black         | #000000      |
| 1     | Dark Blue     | #1D2B53      |
| 2     | Dark Purple   | #7E2553      |
| 3     | Dark Green    | #008751      |
| 4     | Brown         | #AB5236      |
| 5     | Dark Gray     | #5F574F      |
| 6     | Light Gray    | #C2C3C7      |
| 7     | White         | #FFF1E8      |
| 8     | Red           | #FF004D      |
| 9     | Orange        | #FFA300      |
| 10    | Yellow        | #FFEC27      |
| 11    | Green         | #00E436      |
| 12    | Blue          | #29ADFF      |
| 13    | Indigo        | #83769C      |
| 14    | Pink          | #FF77A8      |
| 15    | Peach         | #FFCCAA      |

---

## Coordinate System

- **Screen size:** 256 x 256 pixels
- **Origin:** (0, 0) = top-left corner
- **X axis:** 0 (left) to 255 (right)
- **Y axis:** 0 (top) to 255 (bottom)

---

## Frame Rate

- **Target:** 60 FPS
- **`_update()` is called:** 60 times per second
- **`_draw()` is called:** 60 times per second

---

## Best Practices

### 1. Keep _update() and _draw() Separate
```lua
-- Good
function _update()
    player.x = player.x + player.vx
end

function _draw()
    cls()
    spr(0, player.x, player.y)
end
```

### 2. Use Local Variables
```lua
-- Faster
local function calc_distance(x1, y1, x2, y2)
    local dx = x2 - x1
    local dy = y2 - y1
    return sqrt(dx*dx + dy*dy)
end
```

### 3. Cache Expensive Calls
```lua
-- Good
local t = time()
local wave = sin(t) * 50

-- Avoid
local wave = sin(time()) * 50  -- Don't call time() multiple times
```

### 4. Use Button Constants
```lua
local BTN_LEFT = 0
local BTN_A = 4

if btn(BTN_LEFT) then move_left() end
if btnp(BTN_A) then jump() end
```

---

## See Also

- `docs/KEYBOARD_AND_CONTROLLER.md` - Input reference
- `docs/SPRITE_FORMAT.md` - Sprite data format
- `docs/MAP_FORMAT.md` - Map data format
- `docs/SFX_FORMAT.md` - Sound effect format
- `docs/MUSIC_FORMAT.md` - Music format
- `cartridges/api_test/main.lua` - API test examples

---

**ULICS v1.0**  
**API Reference Complete**  
**40 Functions - Fantasy Console Perfection**
