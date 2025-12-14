# ULICS Fantasy Console v1.1.0 - "Stable Foundation"

[![Version](https://img.shields.io/badge/version-1.1.0-blue.svg)](https://github.com/UlisesFreitas/ULICS)
[![Status](https://img.shields.io/badge/status-stable-brightgreen.svg)](https://github.com/UlisesFreitas/ULICS)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Released:** 2025-12-14  
**A high-performance fantasy console built with C++ and SDL2**

---

## 🎮 What is ULICS?

ULICS is a **fantasy console** - a virtual game console with self-imposed limitations that encourage creativity. Think PICO-8 or TIC-80, but with modern C++ performance.

### Key Features

✅ **256x256 pixel display** with 16-color palette  
✅ **40+ Lua APIs** for graphics, input, sound, and more  
✅ **Complete input system** - Keyboard, mouse, and gamepad  
✅ **Sprite and map support** - Multi-layer tilemaps (up to 8 layers)  
✅ **Working audio system** - Lock-free synthesis with 8 channels  
✅ **Hot reload** - Edit code and see changes instantly  
✅ **Debug tools** - On-screen console, screenshots, GIF recording  
✅ **60 FPS** smooth performance with zero input lag

---

## 🆕 What's New in v1.1.0

### Bug Fixes ✅
- **Fixed rendering flicker** in Pong demo (floating point coordinates)
- **Fixed player disappearing** in Platformer (same root cause)
- **Fixed critical input lag** (simplified game loop to 60fps direct)
- **Fixed audio threading** (lock-free ring buffer implementation)

### Audio System 🔊
- ✅ Working SFX synthesis with 8 channels
- ✅ 4 waveforms: sine, square, triangle, white noise
- ✅ Soft limiter to prevent clipping
- ✅ Zero input lag with lock-free ring buffer

### Quality of Life (v1.5 features) 🛠️
- ✅ **Hot Reload** - Auto-reload on file save (massive productivity!)
- ✅ **Debug Console** - F1 to toggle on-screen print() output
- ✅ **Screenshot** - F12 to capture PNG
- ✅ **GIF Recording** - Ctrl+F12 for 5-second GIF
- ✅ **Cartridge Generator** - PowerShell script to scaffold new games

---

## 📦 What's Included

### Core Runtime
- **Engine**: C++17 with SDL2
- **Scripting**: Lua 5.4
- **Graphics**: Custom pixel-perfect renderer
- **Input**: Keyboard (100+ keys), Mouse, Gamepad (21 buttons)
- **Audio**: Lock-free synthesis with ring buffer architecture

### Demo Cartridges
1. **hello_world** - Simple moving text
2. **bouncing_ball** - Physics demo
3. **api_test** - API function showcase
4. **basic_sfx** - Audio test ✅ **NOW WORKING**
5. **pong** - Classic 2-player Pong ✅ **FIXED**
6. **platformer** - Coin collection platformer ✅ **FIXED**
7. **audio_test** - SFX synthesis test (new!)
8. **input_test** - Input debugging tool (new!)

### Documentation
- `docs/LUA_API.md` - Complete API reference (40+ functions)
- `docs/KEYBOARD_AND_CONTROLLER.md` - Input guide
- `docs/SPRITE_FORMAT.md` - Sprite data format
- `docs/MAP_FORMAT.md` - Map data format with parallax examples
- `docs/KNOWN_ISSUES_V1.0.md` - Resolution report (all fixed!)
- `docs/CODE_REVIEW_V1.0.md` - Technical review
- + 6 more specification documents

---

## 🚀 Quick Start

### Prerequisites
- Windows 10/11
- CMake 3.20+
- Visual Studio 2019+ or MinGW
- SDL2 (included via submodule)

### Build

```powershell
# Clone repository
git clone https://github.com/UlisesFreitas/ULICS.git
cd ULICS

# Initialize SDL2 submodule
git submodule update --init --recursive

# Build with CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Run
.\bin\Release\UliCS.exe
```

### Create Your First Cartridge

Use the cartridge generator for instant scaffolding:

```powershell
# Create a platformer template
.\tools\new-cartridge.ps1 -Name "my_game" -Template Platformer

# Or simple template
.\tools\new-cartridge.ps1 -Name "my_game" -Template Simple
```

Or manually create `cartridges\my_game\main.lua`:

```lua
function _init()
    x = 128
    y = 128
end

function _update()
    if btn(0) then x = x - 2 end  -- Left
    if btn(1) then x = x + 2 end  -- Right
    if btn(2) then y = y - 2 end  -- Up
    if btn(3) then y = y + 2 end  -- Down
    
    -- Play sound on Z press
    if btnp(4) then sfx(0) end
end

function _draw()
    cls(1)
    circfill(flr(x), flr(y), 8, 8)  -- Always use flr() on coordinates!
    print("HELLO ULICS!", 80, 10, 7)
end
```

**Important:** Always use `flr()` to floor coordinates before rendering!

---

## 📖 API Cheatsheet

### Graphics
```lua
cls(color)              -- Clear screen
pset(x, y, c)           -- Set pixel
line(x1, y1, x2, y2, c) -- Draw line
rect(x, y, w, h, c)     -- Rectangle outline
rectfill(x, y, w, h, c) -- Filled rectangle
circ(x, y, r, c)        -- Circle outline
circfill(x, y, r, c)    -- Filled circle
print(text, x, y, c)    -- Draw text
camera(x, y)            -- Set camera offset
```

### Input
```lua
btn(id)                 -- Check button held
btnp(id)                -- Check button pressed (one frame only)
mouse()                 -- Get mouse state
mousex()                -- Get mouse X
mousey()                -- Get mouse Y
```

### Sprites & Maps
```lua
spr(id, x, y)           -- Draw sprite
sspr(x, y, w, h, dx, dy) -- Draw sprite region
map(cx, cy, sx, sy, w, h, layer) -- Draw map (layer optional)
mget(x, y, layer)       -- Get tile (layer optional)
mset(x, y, id, layer)   -- Set tile (layer optional)
```

### Audio ✅ NEW!
```lua
sfx(id, channel)        -- Play sound effect
                        -- id: 0=sine, 1=square, 2=triangle, 3=noise
                        -- channel: 0-7 (optional, default 0)
-- music() coming in Phase 6!
```

### Math
```lua
sin(x), cos(x)          -- Trig (0-1 range)
atan2(dx, dy)           -- Angle
sqrt(x), abs(x)         -- Math
flr(x), ceil(x)         -- Rounding (USE THIS FOR RENDERING!)
rnd(max)                -- Random
```

### System
```lua
time()                  -- Get elapsed time
reset()                 -- Restart cartridge
exit()                  -- Quit ULICS
load_cartridge(path)    -- Load another cartridge
goto_menu()             -- Return to menu
```

---

## 🎯 Controls

### Keyboard
- **Arrow Keys** - D-pad (buttons 0-3)
- **Z/X/A/S** - Action buttons (4-7)
- **Q/W** - Shoulder buttons (8-9)
- **Enter/Shift** - Start/Select (10-11)
- **F1** - Toggle debug console
- **F12** - Screenshot
- **Ctrl+F12** - Start GIF recording

### Gamepad
- **D-pad** - Movement
- **A/B/X/Y** - Action buttons
- **L/R** - Shoulders
- **Start/Back** - System

### Mouse
- Full position tracking
- 3 button support

---

## 📊 Specifications

| Feature | Spec |
|---------|------|
| Resolution | 256x256 pixels |
| Colors | 16-color palette |
| Sprites | 8x8 to 512x512 |
| Map Size | 128x64 tiles, 8 layers |
| Framerate | 60 FPS (stable, zero lag) |
| Audio | 8 channels @ 44.1kHz, lock-free |
| Code | Lua 5.4 |
| Memory | ~30KB Lua memory typical |

---

## ✅ Stability Report (v1.1.0)

### All Critical Bugs Resolved! 🎉

✅ Pong rendering flicker - **FIXED**  
✅ Platformer player disappearing - **FIXED**  
✅ Audio threading input lag (6-8s) - **FIXED**  
✅ Critical input lag - **FIXED**  
✅ Map API connection - **FIXED**  
✅ PNG loading - **FIXED**  
✅ Poor error messages - **FIXED**

**Verdict:** Production-ready and stable! 🚀

See `docs/KNOWN_ISSUES_V1.0.md` for detailed resolution report.

---

## 🗺️ Roadmap

### ✅ v1.1 (Dec 2025) - COMPLETED
- ✅ Fix all rendering bugs
- ✅ Fix audio threading with lock-free ring buffer
- ✅ Connect map API
- ✅ Better error messages

### ✅ v1.5 (Dec 2025) - COMPLETED
- ✅ Hot reload system
- ✅ Debug console overlay
- ✅ Screenshot/GIF capture
- ✅ Cartridge generator

### 🚧 v2.0 (In Progress)
- [ ] ImGui integration
- [ ] Sprite editor
- [ ] Map editor
- [ ] Code editor with syntax highlighting
- [ ] SFX/Music editors

See `plan-ulics-2.json` for complete roadmap.

---

## 📁 Project Structure

```
ULICS/
├── src/
│   ├── core/          # Engine core (simplified 60fps loop)
│   ├── rendering/     # Graphics system
│   ├── audio/         # Audio system (lock-free ring buffer)
│   ├── input/         # Input handling
│   ├── scripting/     # Lua integration
│   ├── cartridge/     # Cartridge loading
│   ├── ui/            # Debug console
│   └── capture/       # Screenshot/GIF
├── cartridges/        # Game cartridges (8 demos)
├── docs/              # Documentation (10+ files)
├── tools/             # Dev tools (cartridge generator)
├── external/          # Dependencies (SDL2, Lua, stb)
└── build/             # Build output
```

---

## 🛠️ Development

### Version Info
```cpp
#include "core/Version.h"

ULICS::VERSION_MAJOR  // 1
ULICS::VERSION_MINOR  // 1
ULICS::VERSION_PATCH  // 0
ULICS::VERSION_STRING // "1.1.0"
```

### Hot Reload Workflow
1. Edit `main.lua` in your favorite editor
2. Save the file
3. ULICS auto-reloads instantly!
4. No need to restart

### Debug Console
- Press **F1** to toggle
- Shows last 10 `print()` calls
- Real-time FPS counter
- Perfect for debugging without terminal

---

## 🤝 Contributing

Contributions welcome for:
- Bug reports (GitHub Issues)
- Documentation improvements
- New demo cartridges
- v2.0+ features (see `plan-ulics-2.json`)

---

## 📄 License

MIT License - See LICENSE file for details

---

## 🙏 Credits

**Created by:** Ulises Freitas  
**Powered by:** SDL2, Lua, C++17  
**Inspired by:** PICO-8, TIC-80, Fantasy Consoles

---

## 🔗 Links

- **Documentation:** `docs/`
- **API Reference:** `docs/LUA_API.md`
- **Roadmap:** `plan-ulics-2.json`
- **Code Review:** `docs/CODE_REVIEW_V1.0.md`
- **Known Issues (Resolved):** `docs/KNOWN_ISSUES_V1.0.md`

---

**ULICS v1.1.0 - "Stable Foundation"**  
*Making game development simple and fun* ✨🎮

---

Last Updated: 2025-12-14
