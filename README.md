# ULICS - Ulises's Limitless Interactive Coding System

A high-performance fantasy console where **everything is code** and the limits are **generous**.

## 🎮 What is ULICS?

ULICS is a fantasy console inspired by PICO-8 but designed for **ambitious projects**:

- **512MB - 1GB RAM** (vs PICO-8's 2MB)
- **Up to 1,000,000 lines of Lua** (vs PICO-8's 8K tokens)
- **Configurable palettes** (16-256 colors)
- **No artificial constraints** - limits are guidelines, not walls

## 🚀 Quick Start

### Building ULICS

**Windows (PowerShell):**
```powershell
# 1. Build dependencies and project
.\build.ps1

# 2. Run ULICS
.\build\bin\Release\UliCS.exe
```

### Your First Cartridge

Create a directory in `cartridges/my_game/`:

**config.json:**
```json
{
  "name": "My Game",
  "author": "Your Name",
  "version": "1.0",
  "memory_limit_mb": 512,
  "lua_code_limit_lines": 100000,
  "palette_size": 16,
  "framebuffer_width": 256,
  "framebuffer_height": 256
}
```

**main.lua:**
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
end

function _draw()
    cls(0)  -- Clear screen
    circfill(x, y, 8, 8)  -- Draw player
    print("MOVE WITH ARROWS", 60, 10, 7)
end
```

Launch ULICS, select your cartridge, and play!

## 📚 API Reference

See [LUA_API.md](LUA_API.md) for complete API documentation.

### Core Functions

**Graphics:**
- `cls(color)` - Clear screen
- `pset(x, y, color)` - Set pixel
- `line(x1, y1, x2, y2, color)` - Draw line
- `rect(x, y, w, h, color)` - Draw rectangle
- `rectfill(x, y, w, h, color)` - Filled rectangle
- `circ(x, y, radius, color)` - Draw circle
- `circfill(x, y, radius, color)` - Filled circle
- `print(text, x, y, color)` - Draw text
- `camera(x, y)` - Set camera offset

**Input:**
- `btn(id)` - Check if button is held (0=Left, 1=Right, 2=Up, 3=Down, 4=Z, 5=X)
- `btnp(id)` - Check if button was just pressed

**Math:**
- `sin(x)`, `cos(x)`, `atan2(dx, dy)`
- `sqrt(x)`, `abs(x)`
- `flr(x)`, `ceil(x)`
- `rnd(max)` - Random number

**System:**
- `time()` - Get elapsed time
- `exit()` - Quit ULICS
- `reset()` - Reload current cartridge
- `goto_menu()` - Return to menu

## 🎯 Philosophy

### High-Performance Fantasy Console

ULICS is designed for:
- **Ambitious hobby projects** that need room to grow
- **Learning game development** without hitting arbitrary limits
- **Rapid prototyping** with immediate visual feedback
- **Portfolio pieces** that showcase real coding skills

### "Limits are Suggestions"

- Memory and code limits are **configurable per cartridge**
- Warnings shown when approaching limits
- **No crashes** - just helpful statistics
- Perfect for learning: start small, scale up

## 📂 Project Structure

```
ULICS/
├── cartridges/           # Your game cartridges
│   ├── hello_world/      # Simple starter example
│   ├── bouncing_ball/    # Physics demo
│   └── api_test/         # Comprehensive API tests
├── src/                  # ULICS engine source
│   ├── core/             # Engine core
│   ├── rendering/        # Graphics layer
│   ├── scripting/        # Lua integration
│   ├── input/            # Input handling
│   └── cartridge/        # Cartridge loading
├── build.ps1             # Build script
├── LUA_API.md            # API documentation
├── VISION.md             # Design philosophy
└── README.md             # This file
```

## 🛠️ Development

### Requirements

- Windows 10/11
- Visual Studio 2019 Build Tools or newer
- CMake 3.10+
- PowerShell

### Building from Source

```powershell
# Clean build
Remove-Item -Recurse -Force build
cmake -B build -G "Visual Studio 16 2019"
cmake --build build --config Release
```

### Running Tests

Load the `api_test` cartridge from the ULICS menu to verify all API functions.

## 📖 Documentation

- **[LUA_API.md](LUA_API.md)** - Complete Lua API reference
- **[VISION.md](VISION.md)** - Project vision and philosophy
- **[CARTRIDGE_SPEC.md](CARTRIDGE_SPEC.md)** - Cartridge format specification

## 🎨 Example Cartridges

### hello_world
Interactive demo - move text with arrow keys

### bouncing_ball
Physics simulation with 8 bouncing balls

### api_test
Comprehensive test suite for all 27 implemented API functions

## 🚧 Current Status

**Phase 4.5: Stabilization & API Completion** ✅ **COMPLETE**

- ✅ All core graphics functions
- ✅ Input handling (keyboard)
- ✅ Math library
- ✅ System control (exit, reset, goto_menu)
- ✅ Resource tracking and limits
- ✅ Visual error screen
- ✅ Cartridge management system

**Next:** Phase 5 - Audio & Advanced Features

## 🤝 Contributing

ULICS is a personal learning project but suggestions and feedback are welcome!

## 📜 License

MIT License - see [LICENSE](LICENSE) file for details.

---

**Made with ❤️ by Ulises**

*"Think big. Code bigger."*
