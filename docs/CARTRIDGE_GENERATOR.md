# ULICS Cartridge Generator

Quick tool to create new ULICS game cartridges with ready-to-use templates.

## Usage

### Basic Command

```powershell
.\tools\new-cartridge.ps1 -Name "my_game"
```

### With Template

```powershell
.\tools\new-cartridge.ps1 -Name "my_platformer" -Template Platformer
```

### With Author

```powershell
.\tools\new-cartridge.ps1 -Name "my_game" -Author "Your Name" -Template TopDown
```

---

## Templates Available

### 1. Simple (Default)
Basic game with player movement and screen wrapping.
- Arrow keys for movement
- Perfect starting point for any game

### 2. Platformer
Complete platformer template with:
- Gravity and jumping
- Ground collision
- Horizontal movement with friction
- Ready to add levels!

### 3. TopDown
Top-down game template:
- 4-directional movement
- Screen bounds
- Perfect for RPGs, shooters, etc.

### 4. Puzzle
Grid-based puzzle template:
- 8x8 grid system
- Cell rendering
- Perfect for match-3, Tetris, etc.

---

## Parameters

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| `-Name` | Yes | - | Name of your cartridge (will be sanitized) |
| `-Template` | No | Simple | Template to use (Simple/Platformer/TopDown/Puzzle) |
| `-Author` | No | Anonymous | Your name |

---

## Generated Files

Each cartridge includes:

### `config.json`
```json
{
    "name": "Your Game",
    "author": "Anonymous",
    "version": "1.0.0",
    "description": "A new ULICS game",
    "memoryLimit": 512,
    "codeLimit": 100000,
    "paletteSize": 16
}
```

### `main.lua`
Template code with:
- `_init()` function
- `_update()` function  
- `_draw()` function
- Basic game logic

### `README.md`
Documentation template with:
- Game description
- Controls
- Development notes

---

## Examples

### Create a simple game
```powershell
.\tools\new-cartridge.ps1 -Name "space_shooter"
```

### Create a platformer
```powershell
.\tools\new-cartridge.ps1 -Name "super_jump" -Template Platformer -Author "GameDev Studio"
```

### Create a puzzle game
```powershell
.\tools\new-cartridge.ps1 -Name "match_three" -Template Puzzle
```

---

## Hot Reload

ULICS has Hot Reload enabled! Just edit `main.lua` and save - changes appear instantly! 🔥

---

## Next Steps

1. **Edit** `main.lua` to add your game logic
2. **Test** by running UliCS and selecting your cartridge
3. **Iterate** with instant hot reload
4. **Share** your game!

---

## Tips

- Use `print()` to debug - output shows in Debug Console (F1)
- Press F12 for screenshots
- Press Ctrl+F12 to record 5-second GIFs
- See `docs/LUA_API.md` for the complete API reference

---

**Happy Game Development!** 🎮✨
