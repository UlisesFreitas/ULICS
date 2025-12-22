# ULICS Phase 5.5: Integrated Development Tools & IDE Features

## Overview

Phase 5.5 transforms ULICS from a **fantasy console runtime** into a **complete integrated development environment (IDE)**. This phase adds all the tools needed to create games entirely within ULICS, inspired by PICO-8 and TIC-80.

**Status:** Pending (0/17 tasks)  
**Priority:** Post-v1.0 (Optional for initial release)  
**Estimated Time:** 20-30 hours

---

## Task Breakdown (17 tasks)

### Core Infrastructure (HIGH Priority)

#### 5.5.1: ImGui Integration ⭐
- **Description:** Integrate Dear ImGui for development UI overlay
- **Toggle:** F1 key to show/hide dev tools
- **Output:** `src/ui/ImGuiLayer.h/cpp`
- **Complexity:** 7/10
- **Time:** 2-3 hours

#### 5.5.8: Debug Console ⭐
- **Description:** Scrollable console showing print() output, errors, FPS, memory
- **Features:** Auto-scroll, clear button, copy to clipboard
- **Output:** `src/ui/DebugConsole.h/cpp`
- **Complexity:** 6/10
- **Time:** 2 hours

#### 5.5.6: Code Editor ⭐
- **Description:** In-engine Lua code editing with syntax highlighting
- **Features:** Line numbers, auto-indent, error indicators, save to main.lua
- **Library:** TextEditor (imgui-based) or custom
- **Output:** `src/ui/CodeEditor.h/cpp`
- **Complexity:** 8/10
- **Time:** 3-4 hours

---

### Asset Editors (HIGH Priority)

#### 5.5.2: Sprite Editor ⭐
- **Description:** Pixel art editor for 8x8 to 128x128 sprites
- **Features:**
  - Canvas with zoom (1x to 16x)
  - 16-color palette picker
  - Tools: pencil, fill, line, rectangle, circle
  - Preview window
  - Import/export PNG
  - Save to cartridge spritesheet
- **Output:** `src/ui/SpriteEditor.h/cpp`
- **Complexity:** 9/10
- **Time:** 4-5 hours

#### 5.5.3: Map Editor ⭐
- **Description:** Visual tile map editor
- **Features:**
  - Grid view (128x64 cells)
  - Up to 8 layer support
  - Tile placement from spritesheet
  - Layer show/hide/lock
  - Fill tool, copy/paste regions
  - Save to map.json
- **Output:** `src/ui/MapEditor.h/cpp`
- **Complexity:** 9/10
- **Time:** 4-5 hours

#### 5.5.4: SFX Editor
- **Description:** Waveform designer for sound effects
- **Features:**
  - Waveform type selector (square, sine, triangle, noise)
  - ADSR envelope visual editor
  - Frequency sweep controls
  - Preview playback
  - Save to sfx.json
- **Output:** `src/ui/SFXEditor.h/cpp`
- **Complexity:** 8/10
- **Time:** 3-4 hours

#### 5.5.5: Music Tracker
- **Description:** Tracker-style music composition
- **Features:**
  - Pattern editor (16-64 rows)
  - 4 channel view
  - Note+SFX input
  - Tempo/speed controls
  - Pattern sequencer
  - Save to music.json
- **Output:** `src/ui/MusicTracker.h/cpp`
- **Complexity:** 10/10
- **Time:** 5-6 hours

---

### Asset Management (MEDIUM Priority)

#### 5.5.11: Palette Editor
- **Description:** Customize 16-color palette
- **Features:**
  - HSL/RGB color picker
  - Import presets (PICO-8, TIC-80, DB16, Sweetie16)
  - Preview in real-time
  - Save to cartridge config
- **Output:** `src/ui/PaletteEditor.h/cpp`
- **Complexity:** 6/10
- **Time:** 2 hours

#### 5.5.12: Asset Browser
- **Description:** Visual overview of all cartridge assets
- **Features:**
  - Thumbnail grid view
  - Sprites, maps, SFX, music preview
  - Delete, duplicate, rename
  - Click to open in respective editor
- **Output:** `src/ui/AssetBrowser.h/cpp`
- **Complexity:** 7/10
- **Time:** 2-3 hours

#### 5.5.9: Cartridge Manager
- **Description:** Cartridge library management
- **Features:**
  - List all cartridges
  - Create new (with templates)
  - Delete, duplicate
  - Export to .ulics bundle
  - Import from .ulics
- **Output:** `src/ui/CartridgeManager.h/cpp`
- **Complexity:** 7/10
- **Time:** 2-3 hours

---

### Developer Tools (MEDIUM Priority)

#### 5.5.7: Hot Reload
- **Description:** Auto-reload cartridge on file changes
- **Features:**
  - Watch main.lua, config.json, assets
  - Detect changes via file timestamps
  - Call reset() automatically
  - Optional: preserve Lua state
- **Output:** `src/core/HotReload.h/cpp`
- **Complexity:** 6/10
- **Time:** 2 hours

#### 5.5.14: Save State System
- **Description:** Quick save/load game state
- **Keys:** F5 (save), F9 (load)
- **Features:**
  - Serialize Lua globals
  - Save to .state file
  - Useful for testing specific scenarios
- **Output:** `src/core/SaveState.h/cpp`
- **Complexity:** 7/10
- **Time:** 2-3 hours

#### 5.5.13: Performance Profiler
- **Description:** Real-time performance monitoring
- **Features:**
  - CPU/GPU time graphs
  - _update() and _draw() execution time
  - Draw call count
  - Identify bottlenecks
- **Output:** `src/ui/Profiler.h/cpp`
- **Complexity:** 8/10
- **Time:** 3 hours

---

### Quality of Life (LOW Priority)

#### 5.5.15: Screenshot/GIF Capture
- **Description:** Capture gameplay for sharing
- **Keys:** F12 (screenshot), Ctrl+F12 (5-sec GIF)
- **Features:**
  - Save PNG to screenshots/ folder
  - Record GIF with stb_image_write or gifenc
  - Auto-timestamp filenames
- **Output:** `src/capture/Capture.h/cpp`
- **Complexity:** 6/10
- **Time:** 2 hours

#### 5.5.16: Keybinding Editor
- **Description:** Remap controls
- **Features:**
  - Visual button mapper
  - Remap any fantasy button to any key/gamepad button
  - Save to user preferences (not cartridge)
  - Reset to defaults
- **Output:** `src/ui/KeybindingEditor.h/cpp`
- **Complexity:** 6/10
- **Time:** 2 hours

#### 5.5.17: In-Game Help/Cheatsheet
- **Description:** API quick reference
- **Toggle:** F1 (or separate key from dev tools)
- **Features:**
  - List all 40 API functions
  - Search by name
  - Quick examples
  - Copy to clipboard
- **Output:** `src/ui/HelpOverlay.h/cpp`
- **Complexity:** 5/10
- **Time:** 1-2 hours

#### 5.5.10: Export/Publish System
- **Description:** Bundle games as standalone executables
- **Features:**
  - Windows .exe export
  - Optional: WASM for web
  - Embed all assets
  - Remove dev tools from build
- **Output:** `src/export/Exporter.h/cpp`
- **Complexity:** 9/10
- **Time:** 4-5 hours

---

## Implementation Strategy

### Phase 1: Foundation (5-7 hours)
1. **5.5.1:** ImGui Integration
2. **5.5.8:** Debug Console
3. **5.5.7:** Hot Reload

**Goal:** Get dev tools overlay working with basic functionality

### Phase 2: Core Editors (12-15 hours)
1. **5.5.6:** Code Editor
2. **5.5.2:** Sprite Editor
3. **5.5.3:** Map Editor

**Goal:** Build/edit games entirely in-engine

### Phase 3: Audio Tools (8-10 hours)
1. **5.5.4:** SFX Editor
2. **5.5.5:** Music Tracker

**Goal:** Create complete audiovisual games

### Phase 4: Polish & Extras (5-7 hours)
1. **5.5.11:** Palette Editor
2. **5.5.12:** Asset Browser
3. **5.5.14:** Save States
4. **5.5.15:** Capture Tools
5. **5.5.13:** Profiler
6. **5.5.9:** Cartridge Manager
7. **5.5.16:** Keybinding Editor
8. **5.5.17:** Help Overlay
9. **5.5.10:** Export System

---

## Total Time Estimate

**Minimum (core only):** 20-22 hours  
**Complete (all tasks):** 30-35 hours  

---

## Dependencies

### Required Libraries
- **Dear ImGui** (v1.89+) - UI framework
- **TextEditor** (optional) - Code editor component
- **stb_image_write.h** - Screenshot saving
- **gifenc** (optional) - GIF recording

### Existing ULICS Systems
- AestheticLayer (for sprite/map rendering)
- SFXSynthesizer/MusicPlayer (for audio preview)
- ScriptingManager (for Lua state management)
- CartridgeLoader (for asset loading)

---

## Post-Phase 5.5 Benefits

With all tools implemented, ULICS becomes:
- ✅ **Self-contained IDE** (no external editors needed)
- ✅ **Rapid prototyping** (hot reload + in-engine editing)
- ✅ **Beginner-friendly** (visual editors + help system)
- ✅ **Professional workflow** (profiler + save states)
- ✅ **Shareable** (export system + GIF capture)

Similar to PICO-8/TIC-80 but with modern C++ performance.

---

## Alternative: Minimal IDE (v1.5 approach)

If Phase 5.5 is too ambitious for v1.0:

**Keep for v1.0:**
- External editors (VS Code, Aseprite, etc.)
- Hot reload only
- Debug console

**Add in v1.5:**
- ImGui integration
- Sprite/Map editors
- Code editor

**Add in v2.0:**
- Audio editors
- Full IDE features

---

**Version:** 1.0 (Plan)  
**Status:** Pending Implementation  
**Next:** Phase 6 (Finalization) or Phase 5.5 (IDE Tools)
