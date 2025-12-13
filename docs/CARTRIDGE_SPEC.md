# ULICS Cartridge Specification

## Version 1.0

---

## 📦 Cartridge Structure

A ULICS cartridge can be structured in two ways:

### Option 1: Directory-based Cartridge

```
cartridges/
└── my_game/
    ├── main.lua       # REQUIRED: Game code
    └── config.json    # OPTIONAL: Configuration
```

### Option 2: Single File Cartridge

```
cartridges/
└── simple_game.lua    # Single Lua file (uses defaults)
```

**Note**: All game assets (sprites, music, maps) are defined as data within `main.lua`, NOT as external files. This maintains the fantasy console philosophy while allowing massive projects.

---

## ⚙️ config.json Specification

### Complete Example

```json
{
  "name": "Epic Space RPG",
  "author": "Studio Name",
  "version": "2.1.0",
  "description": "A massive procedurally generated space adventure with thousands of quests and dynamic AI",
  
  "memory_limit_mb": 768,
  "lua_code_limit_lines": 500000,
  
  "palette_size": 128,
  "framebuffer_width": 256,
  "framebuffer_height": 256,
  
  "target_fps": 60
}
```

### Field Reference

#### Metadata (Optional)

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `name` | string | "Untitled" | Cartridge display name |
| `author` | string | "Unknown" | Creator name |
| `version` | string | "1.0.0" | Semantic version |
| `description` | string | "" | Brief description |

#### Resource Limits (Optional)

| Field | Type | Default | Range | Description |
|-------|------|---------|-------|-------------|
| `memory_limit_mb` | integer | 512 | 512-1024 | Maximum Lua memory allocation in MB |
| `lua_code_limit_lines` | integer | 1000000 | 1-1000000 | Maximum lines of Lua code |

**⚠️ CRITICAL**: These limits are CORE differentiators of ULICS from PICO-8/TIC-80.

#### Graphics Configuration (Optional)

| Field | Type | Default | Options | Description |
|-------|------|---------|---------|-------------|
| `palette_size` | integer | 16 | 16, 32, 64, 128 | Number of colors in palette |
| `framebuffer_width` | integer | 256 | 128-512 | Framebuffer width in pixels |
| `framebuffer_height` | integer | 256 | 128-512 | Framebuffer height in pixels |

#### Performance (Optional)

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `target_fps` | integer | 60 | Target frames per second |

---

## 📝 Configuration Examples

### Minimal Cartridge (Uses All Defaults)

```json
{
  "name": "My First Game",
  "author": "Me"
}
```

Effective configuration:
- memory_limit_mb: 512 (default)
- lua_code_limit_lines: 1000000 (default)
- palette_size: 16 (default)
- framebuffer: 256x256 (default)

### Small Puzzle Game

```json
{
  "name": "Puzzle Master",
  "author": "Puzzle Dev",
  "version": "1.0.0",
  "memory_limit_mb": 512,
  "lua_code_limit_lines": 50000,
  "palette_size": 16
}
```

### Medium Roguelike

```json
{
  "name": "Dungeon Crawler",
  "author": "Rogue Studios",
  "version": "2.3.1",
  "description": "Procedural dungeon crawler with permadeath",
  "memory_limit_mb": 640,
  "lua_code_limit_lines": 250000,
  "palette_size": 64,
  "framebuffer_width": 320,
  "framebuffer_height": 240
}
```

### Massive RPG (High-Performance)

```json
{
  "name": "Epic Fantasy World",
  "author": "Big Studio",
  "version": "3.0.0",
  "description": "Massive open-world RPG with 10,000x10,000 procedural world",
  "memory_limit_mb": 1024,
  "lua_code_limit_lines": 1000000,
  "palette_size": 128,
  "framebuffer_width": 384,
  "framebuffer_height": 384,
  "target_fps": 60
}
```

---

## 🔍 Validation Rules

### Required Fields
- **None** - All fields are optional. Cartridge can exist with just `main.lua`

### Field Constraints

```cpp
// Validation pseudo-code
if (memory_limit_mb < 512 || memory_limit_mb > 1024) {
    LOG_WARNING("memory_limit_mb out of range, clamping to 512-1024MB");
    memory_limit_mb = clamp(memory_limit_mb, 512, 1024);
}

if (lua_code_limit_lines < 1 || lua_code_limit_lines > 1000000) {
    LOG_WARNING("lua_code_limit_lines out of range, clamping to 1-1000000");
    lua_code_limit_lines = clamp(lua_code_limit_lines, 1, 1000000);
}

if (palette_size not in {16, 32, 64, 128}) {
    LOG_WARNING("palette_size must be 16, 32, 64, or 128. Using 16.");
    palette_size = 16;
}

if (framebuffer_width < 128 || framebuffer_width > 512) {
    LOG_WARNING("framebuffer_width out of range, clamping to 128-512");
    framebuffer_width = clamp(framebuffer_width, 128, 512);
}

if (framebuffer_height < 128 || framebuffer_height > 512) {
    LOG_WARNING("framebuffer_height out of range, clamping to 128-512");
    framebuffer_height = clamp(framebuffer_height, 128, 512);
}
```

---

## 📂 Cartridge Discovery

The engine scans for cartridges in the following locations:

1. `./cartridges/` directory (relative to executable)
2. Command-line specified path: `UliCS.exe path/to/my_cart`

### Valid Cartridge Patterns

```
✅ cartridges/my_game/main.lua
✅ cartridges/my_game.lua
✅ cartridges/test/main.lua
✅ path/to/custom/location/main.lua
✅ single_file.lua

❌ cartridges/broken_game/      (no main.lua)
❌ cartridges/readme.txt         (not .lua)
```

---

## 🎮 Example Cartridge: Hello World

### Directory: `cartridges/hello_world/`

**main.lua**:
```lua
-- Minimal ULICS cartridge
function _update()
    -- Game logic runs here at 60 FPS
end

function _draw()
    clear(1)  -- Dark blue background
    print("HELLO, ULICS!", 80, 120, 7)  -- White text
end
```

**config.json** (optional):
```json
{
  "name": "Hello World",
  "author": "ULICS Team",
  "version": "1.0.0"
}
```

---

## 🚀 Example Cartridge: Ambitious Project

### Directory: `cartridges/massive_rpg/`

**main.lua** (excerpt):
```lua
-- This cartridge uses ULICS's massive resources

-- 10,000 x 10,000 procedural world
local world = {}
for y = 1, 10000 do
    world[y] = {}
    for x = 1, 10000 do
        world[y][x] = generate_tile(x, y, seed)
    end
end

-- 50,000 unique items
local items = {}
for i = 1, 50000 do
    items[i] = generate_item(i)
end

-- 5,000 quests with dialogue trees
local quests = {}
for i = 1, 5000 do
    quests[i] = {
        dialogue = generate_dialogue_tree(100),  -- 100 nodes each
        conditions = generate_conditions(50),
        rewards = generate_rewards(20)
    }
end

-- Complex AI behavior trees
local ai_behaviors = create_behavior_forest(1000)  -- 1000 behavior trees

function _update()
    update_world_simulation()
    update_ai_for_all_npcs()
    process_quests()
    update_physics()
end

function _draw()
    draw_world_viewport()
    draw_entities()
    draw_ui()
end
```

**config.json**:
```json
{
  "name": "Infinite Quest RPG",
  "author": "Ambitious Dev",
  "version": "1.0.0",
  "description": "Massive procedural RPG pushing ULICS to its limits",
  "memory_limit_mb": 1024,
  "lua_code_limit_lines": 800000,
  "palette_size": 128,
  "framebuffer_width": 384,
  "framebuffer_height": 384
}
```

**This would be IMPOSSIBLE in PICO-8 (8KB code limit).**  
**This is NORMAL in ULICS.**

---

## 🎯 Design Rationale

### Why No External Assets?

1. **Simplicity**: Single file (or directory with main.lua) = entire game
2. **Portability**: Easy to share, backup, version control
3. **Fantasy Console Philosophy**: Everything is code/data
4. **No File I/O Complexity**: No paths, loading errors, missing files
5. **Creative Challenge**: Encourages procedural generation and clever data encoding

### Why Massive Limits?

1. **Differentiation**: ULICS is NOT PICO-8/TIC-80
2. **Ambitious Projects**: Enable complex RPGs, simulations, AI
3. **Modern Hardware**: We have the resources, why limit creativity?
4. **Configurable**: Developers choose their own constraints
5. **Retro Aesthetic**: Still maintains low-res, palette-based look

---

**Document Version**: 1.0  
**Date**: 2025-12-11  
**Status**: Official Specification
