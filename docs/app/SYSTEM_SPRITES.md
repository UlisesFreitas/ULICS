# System Sprites - UI Icon System

## 📋 Overview

SystemSprites provides a unified icon system for all UI elements in ULICS.  
Instead of text labels, we use **8x8 pixel sprites** for a consistent retro aesthetic.

---

## 🎨 How It Works

### 1. **Auto-Generated Defaults**
On first run, the engine generates `assets/system_sprites.png` with basic icons:

| ID | Icon | Name | Usage |
|----|------|------|-------|
| 0 | 🖊️ | PENCIL | Sprite editor pencil tool |
| 1 | 🪣 | FILL | Flood fill tool |
| 2 | 📏 | LINE | Line drawing tool |
| 3 | ▭ | RECT | Rectangle tool |
| 4 | 💧 | PICKER | Color picker/eyedropper |
| 5 | 💾 | SAVE | Save button/icon |
| 6 | 📂 | LOAD | Load/open folder |
| 7 | ▶️ | PLAY | Play/run game |
| 8 | ⏹️ | STOP | Stop game |
| 9 | ◀️ | ARROW_LEFT | Navigate left |
| 10 | ▶️ | ARROW_RIGHT | Navigate right |
| 11 | 🔼 | ARROW_UP | Navigate up |
| 12 | 🔽 | ARROW_DOWN | Navigate down |
| 13 | 📁 | FOLDER | Folder icon |
| 14 | 📄 | FILE | File/document icon |
| 15 | ⚙️ | GEAR | Settings/config |

### 2. **Editable by User**
You can customize any icon:

**Option A: Image Editor**
```bash
# Open in any editor that supports PNG
paint assets/system_sprites.png
photoshop assets/system_sprites.png
```

**Option B: Sprite Editor (Recommended!)**
1. Copy `assets/system_sprites.png` to a cartridge folder
2. Open ULICS Sprite Editor (F2)
3. Edit the sprites you want to change
4. Save (Ctrl+S)
5. Copy back to `assets/` folder

---

## 💻 Usage in Code

### Basic Drawing

```cpp
#include "ui/SystemSprites.h"

// In Engine or UI class
SystemSprites* systemSprites;

void Initialize() {
    systemSprites = new SystemSprites();
    systemSprites->Initialize();  // Loads PNG or generates defaults
}

void Render(AestheticLayer& renderer) {
    // Draw a single icon
    systemSprites->DrawSprite(renderer, 
        static_cast<int>(SystemSprites::Icon::PENCIL), 
        x, y);
    
    // Draw with scaling
    systemSprites->DrawSprite(renderer, 0, x, y, 2);  // 2x scale
}
```

### In SpriteEditor Toolbar

```cpp
void SpriteEditor::RenderToolbar(AestheticLayer& renderer) {
    int x = TOOLBAR_X;
    
    // Instead of text "P", draw pencil icon
    if (currentTool == Tool::PENCIL) {
        renderer.RectFill(x, TOOLBAR_Y, 16, 16, COLOR_YELLOW);
    }
    systemSprites->DrawSprite(renderer, 
        static_cast<int>(SystemSprites::Icon::PENCIL), 
        x + 4, TOOLBAR_Y + 4);
    
    x += 20;
    
    // Fill icon
    if (currentTool == Tool::FILL) {
        renderer.RectFill(x, TOOLBAR_Y, 16, 16, COLOR_YELLOW);
    }
    systemSprites->DrawSprite(renderer, 
        static_cast<int>(SystemSprites::Icon::FILL), 
        x + 4, TOOLBAR_Y + 4);
    
    // ... etc for other tools
}
```

---

## 🎯 File Format

**File:** `assets/system_sprites.png`  
**Size:** 128x128 pixels  
**Format:** RGBA PNG  
**Grid:** 16x16 sprites (256 total)  
**Sprite Size:** 8x8 pixels each  
**Colors:** Grayscale (0-240) representing palette indices 0-15

### Structure
```
┌─────────────────────────┐
│ 0  1  2  3  ... 15     │  Row 0: UI icons (reserved)
│ 16 17 18 ... 31        │  Row 1: Available
│ 32 33 34 ... 47        │  Row 2: Available
│ ...                     │  ...
│ 240 241 ... 255        │  Row 15: Available
└─────────────────────────┘
```

**First 16 sprites (0-15)** are reserved for system UI.  
**Sprites 16-255** are available for custom use.

---

## 🔧 Advanced: Adding New Icons

### 1. Code Method (Procedural)

Edit `SystemSprites.cpp`, add a new generator:

```cpp
void SystemSprites::GenerateMyIcon() {
    uint8_t myIcon[8][8] = {
        {0,7,7,7,7,7,7,0},
        {7,12,12,12,12,12,12,7},
        // ... your 8x8 design
    };
    std::memcpy(sprites[16], myIcon, 64);  // Use slot 16+
}
```

### 2. PNG Method (Visual)

1. Open `assets/system_sprites.png`
2. Navigate to an empty sprite slot (16+)
3. Draw your 8x8 icon
4. Save
5. Use in code: `DrawSprite(renderer, 16, x, y)`

---

## 📊 Palette Reference

The icons use the standard 16-color PICO-8 palette:

| Index | Color | Name |
|-------|-------|------|
| 0 | Black | Transparent |
| 1 | Dark Blue | |
| 2 | Dark Purple | |
| 3 | Dark Green | |
| 4 | Brown | |
| 5 | Dark Gray | |
| 6 | Light Gray | |
| 7 | White | Primary |
| 8 | Red | |
| 9 | Orange | |
| 10 | Yellow | |
| 11 | Green | |
| 12 | Blue | |
| 13 | Indigo | |
| 14 | Pink | |
| 15 | Peach | |

---

## ✅ Best Practices

1. **Keep it simple:** 8x8 is small, avoid detail
2. **Use high contrast:** Light on dark or vice versa
3. **Index 0 = transparent:** Use for empty pixels
4. **Test at 1x and 2x scale:** Icons should be clear at both
5. **Stick to 3-4 colors max:** Per icon for clarity

---

## 🚀 Future Enhancements

- [ ] Animated icons (frame cycling)
- [ ] Icon categories/themes
- [ ] Hot-reload system sprites
- [ ] In-engine icon editor
- [ ] Export individual icons

---

**Happy icon making!** 🎨
