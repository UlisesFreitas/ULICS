# new-cartridge.ps1 - ULICS Cartridge Template Generator
# Usage: .\tools\new-cartridge.ps1 -Name "my_game"

param(
    [Parameter(Mandatory = $true)]
    [string]$Name,
    
    [Parameter(Mandatory = $false)]
    [string]$Author = "Anonymous",
    
    [Parameter(Mandatory = $false)]
    [ValidateSet("Simple", "Platformer", "TopDown", "Puzzle")]
    [string]$Template = "Simple"
)

$ErrorActionPreference = "Stop"

Write-Host "=== ULICS Cartridge Generator ===" -ForegroundColor Cyan
Write-Host ""

# Sanitize name (remove spaces, special chars)
$sanitizedName = $Name -replace '[^a-zA-Z0-9_-]', '_'
$displayName = $Name

# Create cartridge directory
$cartridgePath = Join-Path -Path "cartridges" -ChildPath $sanitizedName

if (Test-Path $cartridgePath) {
    Write-Host "ERROR: Cartridge '$sanitizedName' already exists!" -ForegroundColor Red
    exit 1
}

Write-Host "Creating cartridge: $sanitizedName" -ForegroundColor Green
New-Item -ItemType Directory -Path $cartridgePath | Out-Null

# === Generate config.json ===
$configJson = @"
{
    "name": "$displayName",
    "author": "$Author",
    "version": "1.0.0",
    "description": "A new ULICS game",
    "memoryLimit": 512,
    "codeLimit": 100000,
    "paletteSize": 16
}
"@

$configPath = Join-Path -Path $cartridgePath -ChildPath "config.json"
Set-Content -Path $configPath -Value $configJson -Encoding UTF8
Write-Host "  ✓ Created config.json" -ForegroundColor Gray

# === Generate main.lua based on template ===
$mainLua = switch ($Template) {
    "Platformer" {
        @"
-- $displayName
-- A platformer game template

-- Player state
player = {
    x = 120,
    y = 200,
    vx = 0,
    vy = 0,
    w = 8,
    h = 8,
    grounded = false
}

-- Physics constants
GRAVITY = 0.5
JUMP_FORCE = -8
MOVE_SPEED = 2
FRICTION = 0.8

function _init()
    print("$displayName initialized!")
end

function _update()
    -- Horizontal movement
    if btn(0) then player.vx = -MOVE_SPEED end  -- Left
    if btn(1) then player.vx = MOVE_SPEED end   -- Right
    
    -- Apply friction
    player.vx = player.vx * FRICTION
    
    -- Jump
    if btnp(4) and player.grounded then
        player.vy = JUMP_FORCE
        player.grounded = false
    end
    
    -- Apply gravity
    player.vy = player.vy + GRAVITY
    
    -- Update position
    player.x = player.x + player.vx
    player.y = player.y + player.vy
    
    -- Simple ground collision
    if player.y >= 220 then
        player.y = 220
        player.vy = 0
        player.grounded = true
    end
    
    -- Screen bounds
    player.x = math.max(0, math.min(248, player.x))
end

function _draw()
    cls(1)  -- Clear to dark blue
    
    -- Draw ground
    rectfill(0, 228, 255, 255, 7)
    
    -- Draw player
    rectfill(player.x, player.y, player.x + player.w, player.y + player.h, 11)
    
    -- Instructions
    print("Arrow Keys: Move", 8, 8, 7)
    print("Z: Jump", 8, 16, 7)
end
"@
    }
    
    "TopDown" {
        @"
-- $displayName
-- A top-down game template

-- Player state
player = {
    x = 120,
    y = 120,
    speed = 2,
    size = 8
}

function _init()
    print("$displayName initialized!")
end

function _update()
    -- 4-directional movement
    if btn(0) then player.x = player.x - player.speed end  -- Left
    if btn(1) then player.x = player.x + player.speed end  -- Right
    if btn(2) then player.y = player.y - player.speed end  -- Up
    if btn(3) then player.y = player.y + player.speed end  -- Down
    
    -- Keep player on screen
    player.x = math.max(0, math.min(256 - player.size, player.x))
    player.y = math.max(0, math.min(256 - player.size, player.y))
end

function _draw()
    cls(1)  -- Clear to dark blue
    
    -- Draw player
    rectfill(player.x, player.y, player.x + player.size, player.y + player.size, 11)
    
    -- Instructions
    print("Arrow Keys: Move", 8, 8, 7)
end
"@
    }
    
    "Puzzle" {
        @"
-- $displayName
-- A puzzle game template

-- Game state
grid = {}
gridSize = 8
cellSize = 32

function _init()
    -- Initialize empty grid
    for y = 0, gridSize - 1 do
        grid[y] = {}
        for x = 0, gridSize - 1 do
            grid[y][x] = 0
        end
    end
    
    print("$displayName initialized!")
end

function _update()
    -- Add your puzzle logic here
end

function _draw()
    cls(1)  -- Clear to dark blue
    
    -- Draw grid
    for y = 0, gridSize - 1 do
        for x = 0, gridSize - 1 do
            local px = x * cellSize
            local py = y * cellSize
            
            -- Draw cell
            rect(px, py, px + cellSize - 1, py + cellSize - 1, 7)
            
            -- Draw cell content
            if grid[y][x] > 0 then
                rectfill(px + 2, py + 2, px + cellSize - 3, py + cellSize - 3, grid[y][x])
            end
        end
    end
    
    -- Instructions
    print("Your Puzzle Game", 8, 8, 7)
end
"@
    }
    
    default {
        # Simple
        @"
-- $displayName
-- Created with ULICS Cartridge Generator

-- Game state
player = {
    x = 120,
    y = 120,
    size = 8,
    color = 11
}

function _init()
    print("$displayName initialized!")
    print("Press arrow keys to move")
end

function _update()
    -- Movement
    if btn(0) then player.x = player.x - 2 end  -- Left
    if btn(1) then player.x = player.x + 2 end  -- Right
    if btn(2) then player.y = player.y - 2 end  -- Up
    if btn(3) then player.y = player.y + 2 end  -- Down
    
    -- Screen wrapping
    if player.x < 0 then player.x = 256 end
    if player.x > 256 then player.x = 0 end
    if player.y < 0 then player.y = 256 end
    if player.y > 256 then player.y = 0 end
end

function _draw()
    cls(1)  -- Clear to dark blue
    
    -- Draw player
    rectfill(player.x, player.y, 
             player.x + player.size, player.y + player.size, 
             player.color)
    
    -- Draw title
    print("$displayName", 80, 8, 7)
    
    -- Draw instructions
    print("Arrow Keys: Move", 8, 240, 6)
end
"@
    }
}

$mainLuaPath = Join-Path -Path $cartridgePath -ChildPath "main.lua"
Set-Content -Path $mainLuaPath -Value $mainLua -Encoding UTF8
Write-Host "  ✓ Created main.lua ($Template template)" -ForegroundColor Gray

# === Generate README.md ===
$readme = @"
# $displayName

**Author:** $Author  
**Version:** 1.0.0

## Description

A new ULICS game.

## Controls

- **Arrow Keys** - Movement
- **Z (BTN_A)** - Action 1
- **X (BTN_B)** - Action 2

## Development

Edit `main.lua` to add your game logic.

ULICS will hot-reload when you save!

## API Reference

See `docs/LUA_API.md` for the complete ULICS Lua API.
"@

$readmePath = Join-Path -Path $cartridgePath -ChildPath "README.md"
Set-Content -Path $readmePath -Value $readme -Encoding UTF8
Write-Host "  ✓ Created README.md" -ForegroundColor Gray

Write-Host ""
Write-Host "=== Cartridge Created Successfully! ===" -ForegroundColor Green
Write-Host ""
Write-Host "Location: $cartridgePath" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run your cartridge:" -ForegroundColor Yellow
Write-Host "  .\build\bin\Release\UliCS.exe" -ForegroundColor White
Write-Host "  Then select '$displayName' from the menu" -ForegroundColor White
Write-Host ""
Write-Host "Files created:" -ForegroundColor Yellow
Write-Host "  - config.json (cartridge metadata)" -ForegroundColor Gray
Write-Host "  - main.lua (game code - $Template template)" -ForegroundColor Gray
Write-Host "  - README.md (documentation)" -ForegroundColor Gray
Write-Host ""
Write-Host "Happy coding! 🎮" -ForegroundColor Cyan
