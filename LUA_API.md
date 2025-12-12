# ULICS Fantasy Console - Lua API Reference

This document outlines the complete Lua API available to developers creating cartridges for the ULICS console.

## API Status Legend

| Status | Description |
| :--- | :--- |
| ✅ **Implemented** | The function is complete and available in the current build. |
| 📝 **Planned** | The function is planned for the current development phase. |
| ❌ **Future** | The function is considered for a future development phase. |

---

## Graphics API

Functions for drawing pixels, shapes, and text to the screen.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `clear(c)` | `color` | Clears the screen with a specific color. | ✅ **Implemented** |
| `cls(c)` | `color` | Alias for `clear()`. Clears the screen with a specific color. | ✅ **Implemented** |
| `pset(x, y, c)` | `x`, `y`, `color` | Draws a single pixel. | ✅ **Implemented** |
| `line(x1, y1, x2, y2, c)` | `x1`, `y1`, `x2`, `y2`, `color` | Draws a line between two points. | ✅ **Implemented** |
| `rect(x, y, w, h, c)` | `x`, `y`, `width`, `height`, `color` | Draws the outline of a rectangle. | ✅ **Implemented** |
| `rectfill(x, y, w, h, c)` | `x`, `y`, `width`, `height`, `color` | Draws a filled rectangle. | ✅ **Implemented** |
| `circ(x, y, r, c)` | `x`, `y`, `radius`, `color` | Draws the outline of a circle. | ✅ **Implemented** |
| `circfill(x, y, r, c)` | `x`, `y`, `radius`, `color` | Draws a filled circle. | ✅ **Implemented** |
| `pget(x, y)` | `x`, `y` | Gets the color index of a pixel. | ✅ **Implemented** |
| `print(str, x, y, c)` | `text`, `x`, `y`, `color` | Draws text to the screen. | ✅ **Implemented** |
| `spr(n, x, y, ...)` | `sprite#`, `x`, `y`, `...` | Draws a sprite from the spritesheet. | ❌ **Future** |
| `sspr(...)` | `...` | Draws a scaled section of the spritesheet. | ❌ **Future** |
| `camera(x, y)` | `x`, `y` | Sets the screen's drawing offset (for scrolling). | ✅ **Implemented** |
| `tcolor(c)` | `color_id` | Sets the transparent color index for sprite operations. Pass -1 to disable transparency. | ✅ **Implemented** |
| `pal(c1, c2, p)` | `color1`, `color2`, `...` | Swaps palette colors for screen effects. | ❌ **Future** |

---

## Input API

Functions for reading user input from the keyboard, mouse, and gamepads.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `btn(i)` | `button_id` | Returns `true` if a button is currently held down. | ✅ **Implemented** |
| `btnp(i)` | `button_id` | Returns `true` if a button was just pressed this frame. | ✅ **Implemented** |
| `mouse()` | - | Returns the mouse coordinates and button states. | ❌ **Future** |

---

## Audio API

Functions for playing sound effects and music.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `sfx(n, ...)` | `sound#`, `...` | Plays a sound effect. | ❌ **Future** |
| `music(n, ...)` | `track#`, `...` | Plays a music track. | ❌ **Future** |

---

## Math API

Convenience functions for common mathematical operations.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `sin(x)` | `angle` | Returns the sine of an angle `x`. `x` is from 0-1, representing a full circle. | ✅ **Implemented** |
| `cos(x)` | `angle` | Returns the cosine of an angle `x`. `x` is from 0-1, representing a full circle. | ✅ **Implemented** |
| `atan2(dx, dy)`| `dx`, `dy` | Returns the angle for a given vector, from 0-1. | ✅ **Implemented** |
| `sqrt(x)` | `value` | Returns the square root of `x`. | ✅ **Implemented** |
| `abs(x)` | `value` | Returns the absolute (non-negative) value of `x`. | ✅ **Implemented** |
| `flr(x)` | `value` | Returns the nearest integer equal to or less than `x`. | ✅ **Implemented** |
| `ceil(x)` | `value` | Returns the nearest integer equal to or greater than `x`. | ✅ **Implemented** |
| `rnd(max)` | `max_value` | Returns a random number between 0 (inclusive) and `max` (exclusive). If `max` is omitted, returns a value between 0 and 1. | ✅ **Implemented** |


---

## System API

Functions for interacting with the ULICS system and cartridge management.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `time()` | - | Returns the elapsed time in seconds since the cartridge started. | ✅ **Implemented** |
| `load_cartridge(path)` | `cartridge_path` | Dynamically loads and runs a different cartridge. Used by the system menu. Returns `true` on success. | ✅ **Implemented** |
| `list_cartridges()` | - | Returns a table of available cartridges in the `cartridges/` directory. Each entry has `name` and `path` fields. | ✅ **Implemented** |

---

## Map API

Functions for interacting with the tilemap data.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `mget(x, y)` | `cell_x`, `cell_y` | Gets the sprite ID at a map cell. | ❌ **Future** |
| `mset(x, y, id)` | `cell_x`, `cell_y`, `sprite_id` | Sets the sprite ID at a map cell. | ❌ **Future** |
| `map(...)` | `...` | Draws a section of the map to the screen. | ❌ **Future** |

---

## Memory API

Advanced functions for direct memory manipulation.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `peek(addr)` | `address` | Reads a byte from a memory address. | ❌ **Future** |
| `poke(addr, val)` | `address`, `value` | Writes a byte to a memory address. | ❌ **Future** |
| `dget(i)` / `dset(i, v)` | `index`, `value` | Reads/writes persistent cartridge data. | ❌ **Future** |
