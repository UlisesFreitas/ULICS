# SPRITE EDITOR: FUTURE ENHANCEMENTS ROADMAP

**Version:** 2.0 - Advanced Features  
**Status:** PLANNING  
**Priority:** Medium-High

---

## 📋 Overview

This document outlines 8 future enhancements for the Sprite Editor Import/Export system, prioritized by impact vs. effort.

---

## 🎯 PRIORITY 1: Quick Wins (High Impact, Low Effort)

### 1. Import/Export Hotkeys ⌨️

**Feature:** Add keyboard shortcuts for Import (Ctrl+O) and Export (Ctrl+E)

**Priority:** HIGH  
**Effort:** LOW (30 min)  
**Impact:** HIGH - Dramatically speeds up workflow

#### Implementation:
```cpp
// In SpriteEditor::HandleKeyboard()
if (input.IsCtrlPressed()) {
    if (input.IsKeyPressed(SDLK_o)) {
        OnImportClicked();
    }
    if (input.IsKeyPressed(SDLK_e)) {
        OnExportClicked();
    }
}
```

#### Tasks:
- [ ] Add hotkey detection in HandleKeyboard()
- [ ] Call OnImportClicked() on Ctrl+O
- [ ] Call OnExportClicked() on Ctrl+E
- [ ] Add visual indicator in UI (tooltip or label)
- [ ] Update documentation

**Estimated Time:** 30 minutes  
**Files Modified:** SpriteEditor.cpp, SpriteEditor.h

---

### 2. Recent Files List 📋

**Feature:** Remember last 5 imported files for quick re-import

**Priority:** MEDIUM-HIGH  
**Effort:** LOW (1 hour)  
**Impact:** MEDIUM - Convenience for iterative workflow

#### Design:
```
[📂 Import ▼]
  └─ Recent Files:
     • C:\Projects\sprites_v1.png
     • C:\Downloads\tileset.png
     • ...
  └─ Browse...
```

#### Implementation:
```cpp
class SpriteEditor {
private:
    std::vector<std::string> recentFiles;
    static const int MAX_RECENT = 5;
    
    void AddToRecentFiles(const std::string& path);
    void LoadRecentFiles();  // From config
    void SaveRecentFiles();  // To config
};
```

#### Storage:
- Save in `./config/sprite_editor_recent.txt`
- One path per line
- Max 5 entries

#### Tasks:
- [ ] Add recentFiles vector to SpriteEditor
- [ ] Implement AddToRecentFiles()
- [ ] Load/Save recent files to config
- [ ] Add dropdown UI on Import button click
- [ ] Handle click on recent file entry

**Estimated Time:** 1 hour  
**Files Modified:** SpriteEditor.h, SpriteEditor.cpp  
**Files Created:** config/sprite_editor_recent.txt

---

## 🎯 PRIORITY 2: Moderate Improvements (Medium Impact/Effort)

### 3. Custom Naming Templates 🏷️

**Feature:** User-defined filename templates for exports

**Priority:** MEDIUM  
**Effort:** MEDIUM (1.5 hours)  
**Impact:** MEDIUM - Professional workflow

#### Templates:
```
%project%_sprites_%date%.png         → MyGame_sprites_2025-12-19.png
tileset_%width%x%height%.png         → tileset_128x128.png
%username%_%timestamp%.png           → ulises_235959.png
sprites_v%version%.png               → sprites_v1.png (manual increment)
```

#### Variables:
- `%date%` - YYYY-MM-DD
- `%time%` - HHMMSS
- `%timestamp%` - YYYYMMDD_HHMMSS
- `%project%` - Cartridge folder name
- `%width%` - Image width
- `%height%` - Image height
- `%username%` - System username
- `%version%` - Manual counter

#### UI:
```
Export Spritesheet
┌─────────────────────────────┐
│ Filename template:          │
│ [sprites_%date%.png       ] │
│                             │
│ Preview: sprites_2025-12-19.png
│                             │
│ [Export] [Cancel]           │
└─────────────────────────────┘
```

#### Tasks:
- [ ] Create TemplateParser class
- [ ] Implement variable replacement logic
- [ ] Add template input field to export dialog
- [ ] Save last used template in config
- [ ] Show preview of final filename

**Estimated Time:** 1.5 hours  
**Files Modified:** SpriteEditor.cpp, SpriteEditor.h  
**Files Created:** utils/TemplateParser.h, utils/TemplateParser.cpp

---

### 4. Drag & Drop Import 🖱️

**Feature:** Drag PNG file from Explorer directly onto Sprite Editor window

**Priority:** MEDIUM  
**Effort:** MEDIUM (2 hours)  
**Impact:** MEDIUM-HIGH - Very intuitive UX

#### SDL Implementation:
```cpp
// In Engine::HandleEvents()
case SDL_DROPFILE: {
    char* droppedFile = event.drop.file;
    if (spriteEditor && spriteEditor->IsActive()) {
        spriteEditor->OnFileDropped(droppedFile);
    }
    SDL_free(droppedFile);
    break;
}
```

#### Validation:
- Check file extension (.png)
- Validate dimensions (128×128)
- Show error if invalid
- Auto-import if valid

#### Visual Feedback:
```
┌─────────────────────────┐
│  Drop PNG here to import │
│         (128×128)        │
└─────────────────────────┘
```

#### Tasks:
- [ ] Enable SDL_DROPFILE events in SDL_Init
- [ ] Add HandleDropFile() to Engine
- [ ] Create OnFileDropped() in SpriteEditor
- [ ] Add drag-over visual indicator
- [ ] Validate dropped file
- [ ] Import if valid

**Estimated Time:** 2 hours  
**Files Modified:** Engine.cpp, Engine.h, SpriteEditor.cpp, SpriteEditor.h

---

## 🎯 PRIORITY 3: Advanced Features (High Effort)

### 5. Import Selected Sprites Only 🎯

**Feature:** Import only specific sprites from PNG, not all 256

**Priority:** MEDIUM  
**Effort:** MEDIUM-HIGH (2.5 hours)  
**Impact:** MEDIUM - Useful for partial updates

#### UI Flow:
```
1. Click Import
2. Select PNG
3. Show sprite grid preview
4. User clicks sprites to import (multi-select)
5. Click "Import Selected"
6. Only selected sprites update
```

#### Preview Grid:
```
┌────────────────────────────────┐
│ Select sprites to import:      │
│ ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐ │
│ │0│1│X│3│4│X│6│7│8│9│...      │
│ └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘ │
│                                │
│ Selected: 2, 5                 │
│ [Import Selected] [Cancel]     │
└────────────────────────────────┘
X = Selected for import
```

#### Implementation:
```cpp
struct SpritePreview {
    int index;
    uint8_t data[8][8];
    bool selected;
};

class ImportDialog {
    std::vector<SpritePreview> previews;
    
    void RenderPreviewGrid();
    void HandlePreviewClick(int spriteIndex);
    void ImportSelectedSprites();
};
```

#### Tasks:
- [ ] Create ImportDialog class
- [ ] Render preview grid (16×16 of 8×8 sprites)
- [ ] Handle click selection (toggle)
- [ ] Import only selected indices
- [ ] Add "Select All" / "Deselect All" buttons

**Estimated Time:** 2.5 hours  
**Files Created:** ui/ImportDialog.h, ui/ImportDialog.cpp  
**Files Modified:** SpriteEditor.cpp

---

### 6. Export Individual Sprites 💾

**Feature:** Export single sprite as separate PNG file

**Priority:** MEDIUM  
**Effort:** LOW-MEDIUM (1.5 hours)  
**Impact:** MEDIUM - Useful for sharing individual sprites

#### UI:
```
Right-click on sprite in spritesheet grid
┌─────────────────────┐
│ Export This Sprite  │
│ Copy to Clipboard   │
│ Duplicate Sprite    │
└─────────────────────┘
```

#### Export Options:
- **Size:** 8×8 (original) or 128×128 (16× upscaled)
- **Format:** PNG
- **Filename:** `sprite_#ID_YYYY-MM-DD.png`

#### Implementation:
```cpp
void SpriteEditor::ExportSingleSprite(int spriteIndex, const std::string& filepath) {
    // Export 8×8 sprite
    std::vector<uint8_t> imageData(8 * 8 * 4);
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            uint8_t paletteIndex = spriteSheet[spriteIndex][y][x];
            uint8_t value = paletteIndex * 16;
            
            int pixelIndex = (y * 8 + x) * 4;
            imageData[pixelIndex + 0] = value;
            imageData[pixelIndex + 1] = value;
            imageData[pixelIndex + 2] = value;
            imageData[pixelIndex + 3] = 255;
        }
    }
    
    stbi_write_png(filepath.c_str(), 8, 8, 4, imageData.data(), 8 * 4);
}
```

#### Tasks:
- [ ] Add right-click context menu to spritesheet grid
- [ ] Implement ExportSingleSprite()
- [ ] Add upscale option (8×8 vs 128×128)
- [ ] Default filename with sprite index
- [ ] Show file dialog for save location

**Estimated Time:** 1.5 hours  
**Files Modified:** SpriteEditor.cpp, SpriteEditor.h

---

### 7. Multiple Spritesheet Support 📁

**Feature:** Support multiple spritesheet files per cartridge

**Priority:** LOW-MEDIUM  
**Effort:** HIGH (4 hours)  
**Impact:** HIGH - Major feature for complex games

#### Concept:
```
/cartridge
  /sprites
    - characters.png (256 sprites)
    - tileset.png (256 sprites)
    - ui_icons.png (256 sprites)
    - effects.png (256 sprites)
```

#### UI:
```
┌─────────────────────────────┐
│ Active Spritesheet:         │
│ [characters.png        ▼]   │
│                             │
│ [New] [Load] [Save As]      │
└─────────────────────────────┘
```

#### Data Structure:
```cpp
struct Spritesheet {
    std::string filename;
    uint8_t sprites[256][8][8];
};

class SpriteEditor {
private:
    std::vector<Spritesheet> spritesheets;
    int currentSheetIndex;
    
    void SwitchSpritesheet(int index);
    void NewSpritesheet(const std::string& name);
    void LoadSpritesheet(const std::string& path);
};
```

#### Tasks:
- [ ] Refactor spriteSheet to support multiple sheets
- [ ] Add spritesheet selector dropdown
- [ ] Implement SwitchSpritesheet()
- [ ] Add "New Spritesheet" button
- [ ] Save/Load multiple sheets in cartridge
- [ ] Update all sprite references in game code

**Estimated Time:** 4 hours  
**Files Modified:** SpriteEditor.h, SpriteEditor.cpp, Engine.cpp  
**Impact:** Requires game API changes

---

### 8. Batch Export 📦

**Feature:** Export multiple variations in one operation

**Priority:** LOW  
**Effort:** MEDIUM (2 hours)  
**Impact:** LOW-MEDIUM - Niche use case

#### Use Cases:
- Export at different scales (1×, 2×, 4×, 8×)
- Export with different palettes applied
- Export individual sprites AND full sheet
- Export to multiple formats (PNG, BMP, etc.)

#### UI:
```
Batch Export
┌───────────────────────────────┐
│ ☑ Full spritesheet (128×128)  │
│ ☑ 2× upscaled (256×256)       │
│ ☑ Individual sprites (256 files)│
│ ☐ 4× upscaled (512×512)       │
│                               │
│ Output folder:                │
│ [C:\Export\MySprites     ...]│
│                               │
│ [Export All] [Cancel]         │
└───────────────────────────────┘
```

#### Implementation:
```cpp
struct BatchExportOptions {
    bool fullSheet;
    bool fullSheet2x;
    bool fullSheet4x;
    bool individualSprites;
    std::string outputFolder;
};

void SpriteEditor::BatchExport(const BatchExportOptions& options) {
    if (options.fullSheet) {
        ExportSpritesheet(options.outputFolder + "/spritesheet.png");
    }
    if (options.fullSheet2x) {
        ExportSpritesheetScaled(options.outputFolder + "/spritesheet_2x.png", 2);
    }
    if (options.individualSprites) {
        for (int i = 0; i < 256; i++) {
            ExportSingleSprite(i, options.outputFolder + "/sprite_" + 
                std::to_string(i) + ".png");
        }
    }
}
```

#### Tasks:
- [ ] Create BatchExportDialog UI
- [ ] Add checkboxes for each option
- [ ] Implement ExportSpritesheetScaled()
- [ ] Handle folder selection
- [ ] Show progress bar during export
- [ ] Success summary ("Exported 258 files")

**Estimated Time:** 2 hours  
**Files Created:** ui/BatchExportDialog.h, ui/BatchExportDialog.cpp  
**Files Modified:** SpriteEditor.cpp

---

## 📊 Summary Table

| Feature | Priority | Effort | Time | Impact | ROI |
|---------|----------|--------|------|--------|-----|
| Hotkeys | HIGH | LOW | 30m | HIGH | ⭐⭐⭐⭐⭐ |
| Recent Files | MED-HIGH | LOW | 1h | MEDIUM | ⭐⭐⭐⭐ |
| Custom Templates | MEDIUM | MEDIUM | 1.5h | MEDIUM | ⭐⭐⭐ |
| Drag & Drop | MEDIUM | MEDIUM | 2h | MED-HIGH | ⭐⭐⭐⭐ |
| Selective Import | MEDIUM | MED-HIGH | 2.5h | MEDIUM | ⭐⭐⭐ |
| Export Individual | MEDIUM | LOW-MED | 1.5h | MEDIUM | ⭐⭐⭐ |
| Multiple Sheets | LOW-MED | HIGH | 4h | HIGH | ⭐⭐ |
| Batch Export | LOW | MEDIUM | 2h | LOW-MED | ⭐⭐ |

**ROI = Return on Investment (Impact / Effort)**

---

## 🗓️ Recommended Implementation Order

### Phase 1: Quick Wins (2 hours total)
1. **Hotkeys** (30 min) - Immediate workflow improvement
2. **Recent Files** (1h) - Convenience feature
3. **Export Individual** (30 min) - Simple, useful

### Phase 2: UX Improvements (4 hours total)
4. **Drag & Drop** (2h) - Modern UX expectation
5. **Custom Templates** (1.5h) - Professional feature
6. **Selective Import** (2.5h) - Power user feature

### Phase 3: Advanced (Optional) (6 hours total)
7. **Multiple Sheets** (4h) - Major feature, requires design
8. **Batch Export** (2h) - Nice to have

---

## 🎯 Recommended Next Sprint

**Goal:** Implement Phase 1 (Quick Wins)  
**Time:** 2 hours  
**Features:** Hotkeys + Recent Files + Export Individual

**Benefit:** Maximum user value with minimal time investment

---

## 📋 Implementation Checklist Template

For each feature:
- [ ] Create feature branch
- [ ] Update planning document
- [ ] Implement core logic
- [ ] Add UI elements
- [ ] Add error handling
- [ ] Add logging
- [ ] Test thoroughly
- [ ] Update documentation
- [ ] Merge to main
- [ ] Mark complete

---

## 🔮 Long-term Vision

Eventually, the Sprite Editor could support:
- Animation preview
- Onion skinning
- Layer support
- Palette editing
- Sprite collaboration (real-time)
- Cloud backup/sync
- Undo/redo across spritesheets
- Git integration for sprites

---

*Roadmap created: 2025-12-19*  
*Next Review: When Phase 1 is prioritized*
