# Sprite Editor - Sprite Flags Feature

**Implementation Date:** December 22, 2025  
**Status:** ✅ Completed  
**PICO-8/TIC-80 Compatibility:** Full

---

## 📋 Overview

The **Sprite Flags** system allows developers to assign 8 boolean flags (bits 0-7) to each of the 256 sprites in the spritesheet. These flags can be used to mark sprite properties such as collision, animation state, tile type, or any custom game logic.

This feature is inspired by and fully compatible with PICO-8 and TIC-80 sprite flag systems.

---

## 🎨 User Interface

### Location
The Sprite Flags panel is located **directly below the color palette** in the Sprite Editor (F2).

### Layout
```
┌─────────────────────┐
│ FLAGS               │  ← Label aligned to left border
│ [0][1][2][3]        │  ← First row (flags 0-3)
│ [4][5][6][7]        │  ← Second row (flags 4-7)
└─────────────────────┘
```

### Visual Design
- **Panel:** Dark blue background with white border
- **Label:** "FLAGS" in yellow, aligned to the left edge
- **Checkboxes:** 8x8 pixel boxes arranged in 2 rows × 4 columns
- **Unchecked:** Dark gray background with light gray border
- **Checked:** Yellow "X" mark over checkbox
- **Spacing:** 2px between checkboxes

### Interaction
- **Click** any checkbox to toggle the corresponding flag for the current sprite
- Flags are **sprite-specific** - switching sprites shows different flag states
- Changes are **immediate** and do not require saving (until export)

---

## 💾 File Format

### Storage Location
Sprite flags are stored in a separate `.flags` file alongside the spritesheet:
```
cartridge/
├── spritesheet.png  (128×128 image, 256 sprites)
└── spritesheet.flags (256 bytes, 1 byte per sprite)
```

### Binary Format
- **File size:** Exactly 256 bytes
- **Encoding:** Raw binary (one byte per sprite)
- **Byte order:** Sprite 0 at offset 0, Sprite 255 at offset 255
- **Bit mapping:** Bit 0 = Flag 0, Bit 1 = Flag 1, ..., Bit 7 = Flag 7

Example byte value:
```
Binary:  10101010
Decimal: 170
Flags:   [0]=0, [1]=1, [2]=0, [3]=1, [4]=0, [5]=1, [6]=0, [7]=1
```

### Auto-Save/Load
- **Import Spritesheet:** Automatically loads `spritesheet.flags` if it exists
- **Export Spritesheet:** Automatically saves `spritesheet.flags` alongside PNG
- **Missing File:** If `.flags` file is not found, all flags default to 0

---

## 🔧 C++ API

### SpriteEditor Class Methods

#### Get/Set Individual Flags
```cpp
// Set a single flag bit
void SetSpriteFlag(int spriteIndex, int flagBit, bool value);

// Get a single flag bit
bool GetSpriteFlag(int spriteIndex, int flagBit) const;
```

**Parameters:**
- `spriteIndex`: 0-255 (sprite ID)
- `flagBit`: 0-7 (which flag to access)
- `value`: true/false (for `SetSpriteFlag`)

**Example:**
```cpp
spriteEditor->SetSpriteFlag(42, 0, true);  // Set flag 0 of sprite 42
bool isSolid = spriteEditor->GetSpriteFlag(42, 0);  // Read flag 0
```

#### Get/Set All Flags (Bitmask)
```cpp
// Get all 8 flags as a bitmask (0-255)
uint8_t GetSpriteFlagsAll(int spriteIndex) const;

// Set all 8 flags from a bitmask
void SetSpriteFlagsAll(int spriteIndex, uint8_t flags);
```

**Example:**
```cpp
uint8_t flags = spriteEditor->GetSpriteFlagsAll(10);  // Get all flags
spriteEditor->SetSpriteFlagsAll(10, 0b10101010);  // Set from binary
spriteEditor->SetSpriteFlagsAll(10, 170);         // Same as decimal
```

#### File I/O
```cpp
// Save flags to file
void SaveSpriteFlags(const std::string& flagsPath);

// Load flags from file
void LoadSpriteFlags(const std::string& flagsPath);
```

**Example:**
```cpp
spriteEditor->SaveSpriteFlags("cartridge/spritesheet.flags");
spriteEditor->LoadSpriteFlags("cartridge/spritesheet.flags");
```

---

## 🎮 Lua API (PICO-8 Style)

### `fget()` - Get Sprite Flag(s)

#### Syntax 1: Get Single Flag
```lua
bool fget(sprite_id, flag_bit)
```

Returns `true` if the flag is set, `false` otherwise.

**Example:**
```lua
-- Check if sprite 12 has flag 0 set
if fget(12, 0) then
    -- Sprite is marked as solid
    handle_collision()
end

-- Check multiple flags
local is_animated = fget(5, 4)
local is_hazard = fget(5, 2)
```

#### Syntax 2: Get All Flags
```lua
number fget(sprite_id)
```

Returns a bitmask (0-255) representing all 8 flags.

**Example:**
```lua
-- Get all flags at once
local flags = fget(10)

-- Check specific bit manually
if flags & 1 == 1 then  -- Check flag 0
    print("Flag 0 is set")
end

-- Store and restore flags
local saved_flags = fget(15)
-- ... later ...
fset(15, saved_flags)  -- Restore
```

### `fset()` - Set Sprite Flag(s)

#### Syntax 1: Set Single Flag
```lua
fset(sprite_id, flag_bit, value)
```

Sets a specific flag to `true` or `false`.

**Example:**
```lua
-- Mark sprite 20 as solid
fset(20, 0, true)

-- Clear flag 3 from sprite 8
fset(8, 3, false)

�� Toggle a flag
local current = fget(5, 2)
fset(5, 2, not current)
```

#### Syntax 2: Set All Flags (Bitmask)
```lua
fset(sprite_id, bitmask)
```

Sets all 8 flags at once using a bitmask value (0-255).

**Example:**
```lua
-- Set multiple flags at once
fset(10, 0b10101010)  -- Binary: flags 1,3,5,7 ON

-- Set from decimal
fset(10, 170)  -- Same as above

-- Clear all flags
fset(10, 0)

-- Set all flags
fset(10, 255)
```

---

## 🎯 Common Use Cases

### 1. Collision Detection
```lua
function is_solid(sprite_id)
    return fget(sprite_id, 0)  -- Flag 0 = Solid
end

function check_tile_collision(x, y)
    local tile = mget(x, y)
    if is_solid(tile) then
        -- Handle collision
    end
end
```

### 2. Tile Categories
```lua
-- Flag layout:
-- 0: Solid
-- 1: Platform (one-way)
-- 2: Hazard (spikes, lava)
-- 3: Collectible
-- 4-7: Reserved

function is_platform(sprite_id)
    return fget(sprite_id, 1)
end

function is_hazard(sprite_id)
    return fget(sprite_id, 2)
end
```

### 3. Animation Markers
```lua
-- Flag 4: Is animated
-- Flag 5: Loop animation
-- Flag 6: Reverse animation

function setup_sprite(sprite_id)
    if fget(sprite_id, 4) then
        start_animation(sprite_id)
        
        local should_loop = fget(sprite_id, 5)
        local reverse = fget(sprite_id, 6)
        configure_animation(should_loop, reverse)
    end
end
```

### 4. Enemy Types
```lua
-- Bitmask approach for enemy properties
local ENEMY_FLAGS = {
    CAN_FLY    = 0b00000001,  -- Flag 0
    CAN_SWIM   = 0b00000010,  -- Flag 1
    AGGRESSIVE = 0b00000100,  -- Flag 2
    BOSS       = 0b00001000,  -- Flag 3
}

function create_enemy(sprite_id)
    local flags = fget(sprite_id)
    
    if flags & ENEMY_FLAGS.CAN_FLY > 0 then
        enable_flying()
    end
    
    if flags & ENEMY_FLAGS.BOSS > 0 then
        spawn_boss_music()
    end
end
```

### 5. Batch Flag Operations
```lua
-- Mark all sprites in range as solid
for i = 16, 31 do
    fset(i, 0, true)  -- Set flag 0
end

-- Copy flags from one sprite to another
local template_flags = fget(100)
for i = 101, 110 do
    fset(i, template_flags)
end

-- Clear all flags for water tiles
for i = 64, 79 do
    fset(i, 0)  -- Clear all flags
end
```

---

## 🔍 Technical Details

### Data Structure
```cpp
// In SpriteEditor class
uint8_t spriteFlags[256];  // 8 bits per sprite
```

### Memory Usage
- **256 bytes** total (1 byte × 256 sprites)
- **8 bits per sprite** (flags 0-7)
- **Initialized to 0** on startup

### Performance
- **O(1)** access time for get/set operations
- **Bitwise operations** for flag manipulation
- **No dynamic allocation** - fixed-size array

### Thread Safety
- Not thread-safe (single-threaded use in Sprite Editor)
- Lua API calls are synchronous and safe

---

## 📊 File Format Specification

### `.flags` File Structure
```
Offset | Size  | Description
-------|-------|----------------------------------
0x0000 | 1     | Sprite 0 flags (8 bits)
0x0001 | 1     | Sprite 1 flags (8 bits)
0x0002 | 1     | Sprite 2 flags (8 bits)
...    | ...   | ...
0x00FF | 1     | Sprite 255 flags (8 bits)
-------|-------|----------------------------------
Total  | 256   | 256 bytes
```

### Bit Layout (per byte)
```
Bit:  7  6  5  4  3  2  1  0
Flag: 7  6  5  4  3  2  1  0
```

### Example Hex Dump
```
00000000: 00 00 00 00 FF FF FF FF  01 02 04 08 10 20 40 80  |................|
00000010: AA 55 F0 0F C3 3C 18 24  ...
```

Explanation:
- `0x00-0x03`: Sprites 0-3 have no flags set
- `0x04-0x07`: Sprites 4-7 have all flags set (0xFF = 255 = 0b11111111)
- `0x08-0x0F`: Individual flags set (power of 2 values)
- `0x10`: Sprite 16 has flags 1,3,5,7 set (0xAA = 170 = 0b10101010)
- `0x11`: Sprite 17 has flags 0,2,4,6 set (0x55 = 85 = 0b01010101)

---

## ✅ Implementation Checklist

- [x] Data structure (`spriteFlags[256]`)
- [x] C++ API (Get/Set/Load/Save)
- [x] UI rendering (2×4 grid layout)
- [x] Mouse interaction (click to toggle)
- [x] File I/O (.flags format)
- [x] Auto-load on spritesheet import
- [x] Auto-save on spritesheet export
- [x] Lua API `fget()` function
- [x] Lua API `fset()` function
- [x] Engine getter (`getSpriteEditor()`)
- [x] Documentation (this file)

---

## 🐛 Known Limitations

1. **No undo/redo** for flag changes (unlike pixel edits)
2. **No multi-select** to apply flags to multiple sprites at once
3. **No flag labels** in UI (flags are referenced by number 0-7)
4. **No visual indicator** on spritesheet grid showing which sprites have flags set

---

## 🚀 Future Enhancements

### Potential Improvements (Not Implemented)
- Custom flag labels (user-defined names for each flag)
- Visual overlay on spritesheet showing flagged sprites
- Bulk operations (select multiple sprites, apply flags)
- Flag presets/templates
- Undo/redo support
- Flag search/filter in spritesheet
- Export flags to JSON for readability

---

## 📚 References

- **PICO-8 Documentation:** https://www.lexaloffle.com/dl/docs/pico-8_manual.html#FGET
- **TIC-80 Documentation:** https://github.com/nesbox/TIC-80/wiki/fget
- **Implementation File:** `src/ui/SpriteEditor.cpp` (lines 1826-1960)
- **API File:** `src/scripting/ScriptingManager.cpp` (lines 1034-1086)

---

## 🎓 Example Cartridge

Create a test cartridge to explore sprite flags:

```lua
-- test_flags.lua
-- Demonstration of sprite flags

function _init()
    -- Set up some test sprites with different flags
    fset(0, 0, true)   -- Sprite 0: solid
    fset(1, 1, true)   -- Sprite 1: platform
    fset(2, 2, true)   -- Sprite 2: hazard
    fset(3, 0b1111)    -- Sprite 3: multiple flags (0-3)
    fset(4, 255)       -- Sprite 4: all flags set
end

function _update()
    -- Check for button press to toggle flags
    if btnp(4) then  -- Button Z
        local flag0 = fget(0, 0)
        fset(0, 0, not flag0)
    end
end

function _draw()
    cls()
    
    -- Display sprite flags
    print("SPRITE FLAGS TEST", 10, 10, 7)
    
    for i = 0, 4 do
        local y = 30 + i * 15
        
        -- Draw sprite
        spr(i, 10, y)
        
        -- Show flag bits
        local flags = fget(i)
        local binary = ""
        for bit = 7, 0, -1 do
            if flags & (1 << bit) > 0 then
                binary = binary .. "1"
            else
                binary = binary .. "0"
            end
        end
        
        print("Sprite " .. i .. ": " .. binary .. " (" .. flags .. ")", 30, y + 2, 7)
    end
    
    print("\nPress Z to toggle flag 0 on sprite 0", 10, 100, 6)
end
```

---

**End of Documentation**
