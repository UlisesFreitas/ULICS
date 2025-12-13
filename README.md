# ULICS Fantasy Console v1.0.0 - "First Light"

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/UlisesFreitas/ULICS)
[![Status](https://img.shields.io/badge/status-released-green.svg)](https://github.com/UlisesFreitas/ULICS)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

**Released:** 2025-12-13  
**A high-performance fantasy console built with C++ and SDL2**

---

## 🎮 What is ULICS?

ULICS is a **fantasy console** - a virtual game console with self-imposed limitations that encourage creativity. Think PICO-8 or TIC-80, but with modern C++ performance.

### Key Features

✅ **256x256 pixel display** with 16-color palette  
✅ **40 Lua APIs** for graphics, input, sound, and more  
✅ **Complete input system** - Keyboard, mouse, and gamepad  
✅ **Sprite and map support** - Multi-layer tilemaps  
✅ **Audio architecture** - SDL2 raw audio (synthesis ready)  
✅ **Hot-swappable cartridges** - Load games on-the-fly  
✅ **60 FPS** smooth performance  

---

## 📦 What's Included

### Core Runtime
- **Engine**: C++17 with SDL2
- **Scripting**: Lua 5.4
- **Graphics**: Custom pixel-perfect renderer
- **Input**: Keyboard (100+ keys), Mouse, Gamepad (21 buttons)
- **Audio**: Architecture complete (synthesis in v1.1)

### Demo Cartridges
1. **hello_world** - Simple moving text
2. **bouncing_ball** - Physics demo
3. **api_test** - API function showcase
4. **basic_sfx** - Audio test (disabled pending fix)
5. **pong** - Classic 2-player Pong
6. **platformer** - Coin collection platformer

### Documentation
- `docs/LUA_API.md` - Complete API reference (40 functions)
- `docs/KEYBOARD_AND_CONTROLLER.md` - Input guide
- `docs/SPRITE_FORMAT.md` - Sprite data format
- `docs/MAP_FORMAT.md` - Map data format
- `docs/CODE_REVIEW_V1.0.md` - Technical review
- + 5 more specification documents

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

```powershell
# Create cartridge folder
mkdir cartridges\my_game

# Create config.json
{
  "name": "My Game",
  "author": "Your Name",
  "version": "1.0",
  "description": "My first ULICS game!",
  "palette": "default"
}

# Create main.lua
function _init()
    x = 128
    y = 128
end

function _update()
    if btn(0) then x = x - 2 end  -- Left
    if btn(1) then x = x + 2 end  -- Right
end

function _draw()
    cls(1)
    circfill(x, y, 8, 8)
    print("HELLO ULICS!", 80, 10, 7)
end
```

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
btnp(id)                -- Check button pressed  
mouse()                 -- Get mouse state
mousex()                -- Get mouse X
mousey()                -- Get mouse Y
```

### Sprites & Maps
```lua
spr(id, x, y)           -- Draw sprite
map(cx, cy, sx, sy, w, h) -- Draw map
mget(x, y)              -- Get tile
mset(x, y, id)          -- Set tile
```

### Audio
```lua
sfx(id)                 -- Play sound (v1.1)
music(id)               -- Play music (v1.1)
```

### Math
```lua
sin(x), cos(x)          -- Trig (0-1 range)
atan2(dx, dy)           -- Angle
sqrt(x), abs(x)         -- Math
flr(x), ceil(x)         -- Rounding
rnd(max)                -- Random
```

### System
```lua
time()                  -- Get elapsed time
reset()                 -- Restart cartridge
exit()                  -- Quit ULICS
```

---

## 🎯 Controls

### Keyboard
- **Arrow Keys** - D-pad (buttons 0-3)
- **Z/X/A/S** - Action buttons (4-7)
- **Q/W** - Shoulder buttons (8-9)
- **Enter/Shift** - Start/Select (10-11)

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
| Framerate | 60 FPS (V-Sync) |
| Audio | 8 channels @ 44.1kHz |
| Code | Lua 5.4 |

---

## 🐛 Known Issues (v1.0.0)

### High Priority (v1.1)
- [ ] Pong demo has collision flickering
- [ ] Platformer player sprite disappears
- [ ] Audio synthesis disabled (causes input lag)

### Medium Priority (v1.1)
- [ ] Map API not connected to engine
- [ ] PNG loading uses BMP fallback

See `docs/CODE_REVIEW_V1.0.md` for details.

---

## 🗺️ Roadmap

### v1.1 (Dec 2025)
- Fix rendering bugs
- Fix audio threading
- Connect map API
- Better error messages

### v1.5 (Jan 2026)
- Hot reload
- Debug console
- Screenshot/GIF capture

### v2.0 (Mar 2026)
- Full IDE with ImGui
- Sprite editor
- Map editor
- Code editor
- SFX/Music tracker

See `plan-ulics-2.json` for complete roadmap.

---

## 📁 Project Structure

```
ULICS/
├── src/
│   ├── core/          # Engine core
│   ├── rendering/     # Graphics system
│   ├── audio/         # Audio system
│   ├── input/         # Input handling
│   ├── scripting/     # Lua integration
│   └── cartridge/     # Cartridge loading
├── cartridges/        # Game cartridges
├── docs/              # Documentation
├── external/          # Dependencies (SDL2, Lua)
└── build/             # Build output
```

---

## 🛠️ Development

### Version Info
Access version constants in C++:
```cpp
#include "core/Version.h"

ULICS::VERSION_MAJOR  // 1
ULICS::VERSION_MINOR  // 0
ULICS::VERSION_PATCH  // 0
ULICS::VERSION_STRING // "1.0.0"
```

### Building From Source
```powershell
# Debug build
cmake --build build --config Debug

# Release build (optimized)
cmake --build build --config Release

# Clean build
rm -r build
mkdir build
cd build
cmake ..
```

---

## 🤝 Contributing

ULICS v1.0.0 is feature-complete for the initial release. Contributions welcome for:
- Bug fixes (see GitHub Issues)
- Documentation improvements
- New demo cartridges
- v1.1+ features (see plan-ulics-2.json)

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

---

**ULICS v1.0.0 - "First Light"**  
*Making game development simple again* ✨

---

Last Updated: 2025-12-13
