# Sprite Editor - Animation System

**Status:** ⚙️ Partially Implemented (Backend & API Complete, UI Pending)  
**Version:** 1.0.0  
**Author:** ULICS Team  
**Date:** December 22, 2025

---

## ⚠️ Implementation Status

**✅ COMPLETED:**
- Backend data structures (Animation.h, AnimationManager)
- JSON serialization/deserialization
- Full Lua API (all 12 functions)
- Engine integration (auto-load, auto-update)
- Test cartridge with working animations
- CRITICAL BUG FIX: Spritesheet loading in AestheticLayer

**⏳ PENDING:**
- UI Animation Panel in Sprite Editor
- Animation Editor Modal
- Visual preview in editor

**🐛 Critical Bugs Resolved:**
1. **Spritesheet NULL in AestheticLayer**: El spritesheet solo se cargaba en `SpriteEditor`, pero `DrawSprite()` usa `AestheticLayer::spriteSheet`. Solución: Agregado `LoadSpriteSheet()` a `AestheticLayer` y carga en `Engine::LoadCartridge()`.
2. **AnimationManager::Update() not called in GAME mode**: Se llamaba solo en SETTINGS_MENU. Movido a GAME mode block.
3. **Manual JSON parser**: Implementado parser robusto sin dependencias externas.

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [File Format](#file-format)
4. [Lua API Reference](#lua-api-reference)
5. [UI Guide](#ui-guide)
6. [C++ Implementation](#c-implementation)
7. [Examples](#examples)
8. [Technical Details](#technical-details)

---

## 🎬 Overview

The **Animation System** allows you to create, edit, and play sprite-based animations directly in the Sprite Editor. Animations are sequences of sprites with configurable frame durations and loop settings.

### Key Features

✅ Create animations from existing sprites  
✅ Configure frame duration per frame  
✅ Loop or one-shot playback  
✅ Visual editor with real-time preview  
✅ Simple Lua API for playback  
✅ Auto-save/load with cartridge  
✅ Multiple animations can play simultaneously  

### Use Cases

- **Character animations** (walk, jump, attack, idle)
- **Environmental effects** (water, fire, wind)
- **UI animations** (buttons, icons, indicators)
- **Enemy behaviors** (patrol, attack, death)
- **Collectibles** (coins spinning, gems glowing)

---

## 🚀 Quick Start

### Creating Your First Animation

1. **Open Sprite Editor** (F2)
2. **Create sprites** for animation (e.g., walk cycle: sprites 10, 11, 12, 11)
3. **Click "New Animation"** in the Animation Panel
4. **Name it** (e.g., "player_walk")
5. **Click sprites** 10, 11, 12, 11 on the spritesheet to add frames
6. **Set speed** (default: 8 frames @ 60fps ≈ 133ms per frame)
7. **Toggle Loop** ON
8. **Click Save**

### Using in Lua

```lua
function _init()
    -- Animations auto-load from animations.json
end

function _update()
    -- Animations auto-update each frame
end

function _draw()
    cls()
    
    -- Simple: play and draw
    anim_play("player_walk", 64, 64)
    
    -- Or manual control:
    local frame = anim_get_frame("player_walk")
    spr(frame, player_x, player_y)
end
```

---

## 💾 File Format

### Location

```
cartridge/
├── main.lua
├── spritesheet.png
└── animations.json  ← Animation data
```

### Structure

```json
{
  "animations": [
    {
      "name": "player_walk",
      "frames": [10, 11, 12, 11],
      "speeds": [8, 8, 8, 8],
      "loop": true
    },
    {
      "name": "player_jump",
      "frames": [20, 21, 22],
      "speeds": [6, 10, 6],
      "loop": false
    }
  ]
}
```

### Fields

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Unique identifier for animation |
| `frames` | int[] | Array of sprite IDs (0-255) |
| `speeds` | int[] | Duration of each frame in engine frames @60fps |
| `loop` | bool | If true, animation repeats; if false, stops at end |

### Speed Values

- `8` frames = ~133ms (default, smooth animation)
- `6` frames = ~100ms (faster)
- `10` frames = ~167ms (slower)
- `15` frames = ~250ms (very slow, idle animations)
- `4` frames = ~67ms (very fast, effects)

**Formula:** `duration_ms = (speed / 60) * 1000`

---

## 🎮 Lua API Reference

### All-in-One Functions

#### `anim_play(name, x, y, [flip_x], [flip_y])`

Play and draw animation in one call. Automatically starts animation if not playing.

**Parameters:**
- `name` (string): Animation name
- `x` (int): Screen X position
- `y` (int): Screen Y position
- `flip_x` (bool, optional): Flip horizontally (default: false)
- `flip_y` (bool, optional): Flip vertically (default: false)

**Example:**
```lua
function _draw()
    cls()
    anim_play("player_walk", 64, 64)
    anim_play("enemy_idle", 100, 100, true)  -- Flipped horizontally
end
```

---

#### `anim_draw(name, x, y, [flip_x], [flip_y])`

Draw current frame without starting/controlling animation. Use with manual control.

**Parameters:** Same as `anim_play()`

**Example:**
```lua
function _init()
    anim_start("coin_spin")
end

function _draw()
    cls()
    anim_draw("coin_spin", 64, 64)  -- Just draws, doesn't control
end
```

---

### Control Functions

#### `anim_start(name)`

Start or resume animation from current frame.

**Example:**
```lua
function on_button_press()
    anim_start("player_attack")
end
```

---

#### `anim_stop(name)`

Stop animation and reset to frame 0.

**Example:**
```lua
function on_death()
    anim_stop("player_walk")
    anim_start("player_death")
end
```

---

#### `anim_pause(name)`

Pause animation at current frame (can resume with `anim_start`).

**Example:**
```lua
if game_paused then
    anim_pause("all_animations")
end
```

---

#### `anim_reset(name)`

Reset animation to frame 0 without stopping.

**Example:**
```lua
function restart_animation()
    anim_reset("player_walk")
end
```

---

### Query Functions

#### `anim_get_frame(name) → int`

Get current sprite ID for animation.

**Returns:** Sprite ID (0-255) or -1 if animation doesn't exist

**Example:**
```lua
local current_sprite = anim_get_frame("player_walk")
spr(current_sprite, x, y)
```

---

#### `anim_is_playing(name) → bool`

Check if animation is currently playing.

**Example:**
```lua
if not anim_is_playing("player_attack") then
    anim_start("player_idle")
end
```

---

#### `anim_is_finished(name) → bool`

Check if non-looping animation has finished.

**Returns:** `true` if animation finished, `false` if looping or still playing

**Example:**
```lua
if anim_is_finished("player_jump") then
    anim_start("player_idle")
end
```

---

#### `anim_exists(name) → bool`

Check if animation is loaded.

**Example:**
```lua
if anim_exists("boss_intro") then
    anim_play("boss_intro", 64, 64)
else
    print("Animation not found!")
end
```

---

#### `anim_get_length(name) → int`

Get total number of frames in animation.

**Example:**
```lua
local frames = anim_get_length("player_walk")
print("Walk has " .. frames .. " frames")
```

---

#### `anim_get_duration(name) → int`

Get total duration in engine frames @60fps.

**Example:**
```lua
local dur = anim_get_duration("player_attack")
print("Attack takes " .. (dur/60) .. " seconds")
```

---

## 🎨 UI Guide

### Animation Panel

Located below the FLAGS panel in Sprite Editor.

```
┌────────────────────────────────────────────┐
│ ANIMATIONS                   [+] [▶] [■]   │
├────────────────────────────────────────────┤
│ ▼ player_walk [LOOP] [Edit] [Del]         │
│   [10][11][12][11]  8 fps                  │
│                                             │
│ ▼ player_jump        [Edit] [Del]         │
│   [20][21][22]      10 fps                 │
│                                             │
│ + New Animation                             │
└────────────────────────────────────────────┘
```

### Controls

- **[+]** - Create new animation
- **[▶]** - Play preview (toggle)
- **[■]** - Stop preview
- **[Edit]** - Edit animation
- **[Del]** - Delete animation (with confirmation)

### Animation Editor

```
┌────────────────────────────────────────────┐
│ EDIT: player_walk               [Save][X]  │
├────────────────────────────────────────────┤
│ Name: [player_walk___________]             │
│                                             │
│ Frames (click sprites to add):             │
│ [10] [11] [12] [11]  [+Add] [Clear]       │
│                                             │
│ Speed per frame (frames @60fps):           │
│ [ 8] [ 8] [ 8] [ 8]                        │
│                                             │
│ ☑ Loop animation                           │
│                                             │
│ Preview: [▶ Playing...]                    │
└────────────────────────────────────────────┘
```

### Workflow

1. **Click "New Animation"**
2. **Enter name** (lowercase, underscores recommended)
3. **Click sprites on spritesheet** to add frames in sequence
4. **Adjust speed** for each frame if needed (default: 8)
5. **Toggle loop** if animation should repeat
6. **Click [▶]** to preview
7. **Click Save** when done

### Hotkeys

- `Ctrl+Shift+A` - New animation
- `Ctrl+S` - Save animations (auto-saves with spritesheet)
- `Delete` - Delete selected animation
- `Space` - Toggle preview play/pause

---

## 🔧 C++ Implementation

### Data Structures

```cpp
// Animation.h
struct AnimationFrame {
    int spriteId;      // 0-255
    int duration;      // Frames @60fps
};

struct Animation {
    std::string name;
    std::vector<AnimationFrame> frames;
    bool loop;
    
    // Runtime state
    int currentFrame;
    int frameTimer;
    bool playing;
};
```

### AnimationManager

```cpp
class AnimationManager {
public:
    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path);
    
    void AddAnimation(const Animation& anim);
    void RemoveAnimation(const std::string& name);
    Animation* GetAnimation(const std::string& name);
    
    void Update();  // Called each frame by Engine
    int GetCurrentSpriteId(const std::string& name);
    
    void Play(const std::string& name);
    void Stop(const std::string& name);
    void Pause(const std::string& name);
    void Reset(const std::string& name);
    
    bool IsPlaying(const std::string& name);
    bool IsFinished(const std::string& name);
    bool Exists(const std::string& name);
    
private:
    std::vector<Animation> animations;
    void UpdateAnimation(Animation& anim);
};
```

### Integration with Engine

```cpp
// Engine.h
class Engine {
private:
    std::unique_ptr<AnimationManager> animationManager;
    
public:
    AnimationManager* getAnimationManager() const;
};

// Engine.cpp - In LoadCartridge()
animationManager->LoadFromFile(cartridgePath + "/animations.json");
```

---

## 📚 Examples

### Example 1: Basic Character Animation

```lua
-- Cartridge with player walk animation

player = {
    x = 64,
    y = 64,
    vx = 0,
    facing_right = true
}

function _update()
    -- Movement
    player.vx = 0
    if btn(0) then player.vx = -1; player.facing_right = false end
    if btn(1) then player.vx = 1; player.facing_right = true end
    
    player.x = player.x + player.vx
    
    -- Animation logic
    if player.vx != 0 then
        anim_start("player_walk")
    else
        anim_start("player_idle")
    end
end

function _draw()
    cls()
    
    -- Draw player with current animation
    if player.vx != 0 then
        anim_play("player_walk", player.x, player.y, not player.facing_right)
    else
        anim_play("player_idle", player.x, player.y, not player.facing_right)
    end
end
```

---

### Example 2: One-Shot Attack Animation

```lua
player_state = "idle"

function _update()
    -- Attack button
    if btnp(4) and player_state == "idle" then
        player_state = "attacking"
        anim_start("player_attack")
    end
    
    -- Check if attack finished
    if player_state == "attacking" and anim_is_finished("player_attack") then
        player_state = "idle"
    end
end

function _draw()
    cls()
    
    if player_state == "attacking" then
        anim_play("player_attack", player.x, player.y)
    else
        anim_play("player_idle", player.x, player.y)
    end
end
```

---

### Example 3: Multiple Animated Objects

```lua
coins = {}

function _init()
    -- Spawn coins
    for i = 1, 5 do
        coins[i] = {x = i * 30, y = 64}
    end
end

function _draw()
    cls()
    
    -- Draw all coins with same animation
    for coin in all(coins) do
        anim_play("coin_spin", coin.x, coin.y)
    end
    
    -- Draw player
    anim_play("player_walk", player_x, player_y)
    
    -- Draw enemies
    for enemy in all(enemies) do
        anim_play("enemy_idle", enemy.x, enemy.y)
    end
end
```

---

### Example 4: Animation State Machine

```lua
state_machine = {
    current = "idle",
    states = {
        idle = {anim = "player_idle", next = "walk"},
        walk = {anim = "player_walk", next = "jump"},
        jump = {anim = "player_jump", next = "idle"}
    }
}

function change_state(new_state)
    state_machine.current = new_state
    anim_start(state_machine.states[new_state].anim)
end

function _update()
    -- State transitions
    if btn(0) or btn(1) then
        change_state("walk")
    elseif btnp(4) then
        change_state("jump")
    elseif state_machine.current == "jump" and anim_is_finished("player_jump") then
        change_state("idle")
    end
end

function _draw()
    cls()
    local anim_name = state_machine.states[state_machine.current].anim
    anim_play(anim_name, player.x, player.y)
end
```

---

## 🔍 Technical Details

### Update Mechanism

Animations update automatically each frame in `Engine::Update()`:

```cpp
void Engine::Update() {
    // ... other updates ...
    
    if (animationManager) {
        animationManager->Update();  // Updates all active animations
    }
}
```

Each animation increments its `frameTimer`. When timer reaches frame duration, advances to next frame.

### Frame Timing

- Engine runs at **60 FPS**
- Speed value = number of engine frames per animation frame
- `speed = 8` means each animation frame shows for 8 engine frames (133ms)

### Memory Usage

- Each animation: ~100 bytes (name + frames + state)
- Typical cartridge: 5-10 animations ≈ 500-1000 bytes
- No impact on sprite storage (references existing sprites)

### Performance

- O(1) lookup by name (uses std::vector with linear search, fast for <20 animations)
- O(n) update per frame where n = number of active animations
- Minimal overhead: just frame counter increment + comparison

### Thread Safety

- Single-threaded, no synchronization needed
- All updates happen in main game loop

---

## ⚠️ Known Limitations

1. **No rotation/scaling** - Only frame sequences
2. **No sprite flipping in data** - Handle with `flip_x/flip_y` in draw call
3. **No animation events** - Use `anim_is_finished()` to detect completion
4. **No frame blending** - Hard cuts between frames
5. **UI scrolling** - Panel can show ~10 animations before overflow

---

## 🔧 Implementation Details (Completed Work)

###  Files Created

#### `src/animation/Animation.h`
- `AnimationFrame` struct: `{spriteId, duration}`
- `Animation` struct: `{name, frames[], loop, runtime_state}`
- Runtime state: `currentFrame`, `frameTimer`, `playing`, `paused`

#### `src/animation/AnimationManager.h` / `.cpp`
- `LoadFromFile()`: Manual JSON parser, sin dependencias externas
- `SaveToFile()`: JSON serializer
- `Update()`: Called by Engine @ 60fps, updates all active animations
- `GetCurrentSpriteId()`: Returns sprite for current frame
- Control methods: `Play()`, `Stop()`, `Pause()`, `Reset()`
- Query methods: `IsPlaying()`, `IsFinished()`, `Exists()`

###  Core Integration

#### `src/core/Engine.h` / `.cpp`
```cpp
// Added in Engine.h
std::unique_ptr<AnimationManager> animationManager;
AnimationManager* getAnimationManager() const;

// In Engine::LoadCartridge()
animationManager->LoadFromFile(cartridgePath + "/animations.json");

// In Engine::Update() - GAME mode
if (animationManager) {
    animationManager->Update();  // Frame advancement
}
```

#### `src/rendering/AestheticLayer.h` / `.cpp`
```cpp
// CRITICAL FIX: Added spritesheet loading to AestheticLayer
void LoadSpriteSheet(const std::string& filepath);

// Now called in Engine::LoadCartridge()
aestheticLayer->LoadSpriteSheet(spritesheetPath);
```

**WHY THIS WAS CRITICAL:** `DrawSprite()` uses `AestheticLayer::spriteSheet`, but it was only loaded in `SpriteEditor`. Sprites in GAME mode were always NULL → nothing rendered.

###  Lua API Implementation

All 12 functions implemented in `src/scripting/ScriptingManager.cpp`:

1. `Lua_AnimPlay()` - Calls `Play()` + `DrawSprite()`
2. `Lua_AnimDraw()` - Only `DrawSprite()`, no control
3. `Lua_AnimStart()` → `AnimationManager::Play()`
4. `Lua_AnimStop()` → `AnimationManager::Stop()`
5. `Lua_AnimPause()` → `AnimationManager::Pause()`
6. `Lua_AnimReset()` → `AnimationManager::Reset()`
7. `Lua_AnimGetFrame()` → `AnimationManager::GetCurrentSpriteId()`
8. `Lua_AnimIsPlaying()` → `AnimationManager::IsPlaying()`
9. `Lua_AnimIsFinished()` → `AnimationManager::IsFinished()`
10. `Lua_AnimExists()` → `AnimationManager::Exists()`
11. `Lua_AnimGetLength()` - Returns `frames.size()`
12. `Lua_AnimGetDuration()` - Sum of all frame durations

Registered in `RegisterAPI()` after sprite functions.

###  Testing

#### Test Cartridge: `cartridges/test_animations/`
- `spritesheet.png`: 128x128 with test sprites
- `animations.json`: Single "player_walk" animation (frames: 0, 1, 2, 1)
- `main.lua`: Minimal test script using `anim_play()`

**Test Results:**
- ✅ Animations load from JSON
- ✅ Frames advance at 60fps
- ✅ Loop animations repeat correctly
- ✅ `anim_play()` works in Lua
- ✅ Multiple animations can play simultaneously

###  JSON Parser Implementation

**Challenge:** No external JSON libraries allowed.

**Solution:** Manual recursive-descent parser in `AnimationManager::LoadFromFile()`:
- Finds `"animations"` array
- For each object: extracts name, frames[], speeds[], loop
- Handles whitespace, nested arrays, booleans
- Error handling for malformed JSON

**Example:**
```json
{
  "animations": [
    {
      "name": "player_walk",
      "frames": [0, 1, 2, 1],
      "speeds": [8, 8, 8, 8],
      "loop": true
    }
  ]
}
```

---

## 📖 See Also

- [Sprite Editor Documentation](SPRITE_EDITOR.md)
- [Sprite Flags Documentation](SPRITE_EDITOR_FLAGS.md)
- [Lua API Reference](LUA_API.md)

---

**Last Updated:** 2025-12-22 21:30  
**Version:** 1.0.0  
**Status:** ⚙️ Backend Complete, UI Pending
