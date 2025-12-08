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
| `pset(x, y, c)` | `x`, `y`, `color` | Draws a single pixel. | ✅ **Implemented** |
| `line(x1, y1, x2, y2, c)` | `x1`, `y1`, `x2`, `y2`, `color` | Draws a line between two points. | ✅ **Implemented** |
| `rect(x, y, w, h, c)` | `x`, `y`, `width`, `height`, `color` | Draws the outline of a rectangle. | ✅ **Implemented** |
| `rectfill(x, y, w, h, c)` | `x`, `y`, `width`, `height`, `color` | Draws a filled rectangle. | ✅ **Implemented** |
| `circ(x, y, r, c)` | `x`, `y`, `radius`, `color` | Draws the outline of a circle. | 📝 **Planned** |
| `circfill(x, y, r, c)` | `x`, `y`, `radius`, `color` | Draws a filled circle. | 📝 **Planned** |
| `pget(x, y)` | `x`, `y` | Gets the color index of a pixel. | 📝 **Planned** |
| `print(str, x, y, c)` | `text`, `x`, `y`, `color` | Draws text to the screen. | 📝 **Planned** |
| `spr(n, x, y, ...)` | `sprite#`, `x`, `y`, `...` | Draws a sprite from the spritesheet. | ❌ **Future** |
| `sspr(...)` | `...` | Draws a scaled section of the spritesheet. | ❌ **Future** |
| `camera(x, y)` | `x`, `y` | Sets the screen's drawing offset (for scrolling). | ❌ **Future** |
| `pal(c1, c2, p)` | `color1`, `color2`, `...` | Swaps palette colors for screen effects. | ❌ **Future** |

---

## Input API

Functions for reading user input from the keyboard, mouse, and gamepads.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `btn(i)` | `button_id` | Returns `true` if a button is currently held down. | 📝 **Planned** |
| `btnp(i)` | `button_id` | Returns `true` if a button was just pressed this frame. | 📝 **Planned** |
| `mouse()` | - | Returns the mouse coordinates and button states. | ❌ **Future** |

---

## Audio API

Functions for playing sound effects and music.

| Function | Parameters | Description | Status |
| :--- | :--- | :--- | :--- |
| `sfx(n, ...)` | `sound#`, `...` | Plays a sound effect. | ❌ **Future** |
| `music(n, ...)` | `track#`, `...` | Plays a music track. | ❌ **Future** |

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



