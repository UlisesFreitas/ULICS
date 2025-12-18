# GLOBAL UI SYSTEM - TOPBAR & NAVIGATION

**Project:** ULICS Global Interface  
**Status:** PLANNING  
**Priority:** HIGH  
**Estimated Time:** 2-3 hours

---

## 🎯 OBJECTIVE

Create a unified, professional top bar UI system that:
- Allows easy navigation between all editors (Code, Sprites, Map, SFX, Music, etc.)
- Uses icons instead of text for clean aesthetic
- Remains persistent across all application modes
- Follows PICO-8/TIC-80 retro fantasy console aesthetic

---

## 📐 DESIGN SPECIFICATION

### Layout

```
┌─────────────────────────────────────────────────────────────┐
│ [ULICS] [💾] [📁]  │  [</>] [🎨] [🗺️] [🎵] [🔊] [⚙️]  │  [?] │
│  Logo  Save Open      Code Spr Map Mus SFX  Set      Help  │
└─────────────────────────────────────────────────────────────┘
```

**Dimensions:**
- Height: 16-20 pixels
- Full screen width
- Fixed position: Always at top
- Background: Dark gray/PICO-8 dark blue

---

## 🎨 ICON DESIGN

All icons will be **8×8 sprites** from SystemSprites:

| ID | Icon | Name | Function | Hotkey |
|----|------|------|----------|--------|
| 0-4 | (Reserved) | Tools | Sprite editor tools | - |
| 5 | 💾 | SAVE | Save current file | Ctrl+S |
| 6 | 📂 | LOAD | Open file browser | Ctrl+O |
| 7 | ▶️ | PLAY | Run cartridge | F5 |
| 8 | ⏹️ | STOP | Stop execution | ESC |
| 16 | </> | CODE | Code Editor | F1 |
| 17 | 🎨 | SPRITES | Sprite Editor | F2 |
| 18 | 🗺️ | MAP | Map Editor | F3 |
| 19 | 🎵 | MUSIC | Music Editor | F4 |
| 20 | 🔊 | SFX | Sound Editor | F5 |
| 21 | ⚙️ | SETTINGS | Settings | F6 |
| 22 | ? | HELP | Help/Docs | F12 |

**New Icons to Create:**
- CODE (</> symbol)
- MAP (grid pattern)
- MUSIC (music notes)
- SFX (waveform)
- SETTINGS (gear)
- HELP (question mark)

---

## 🏗️ ARCHITECTURE

### Class Structure

```cpp
class GlobalTopBar {
public:
    void Initialize(SystemSprites* sprites);
    void Update(InputManager& input);
    void Render(AestheticLayer& renderer);
    
    // Navigation
    EditorMode GetRequestedMode() const;
    void ClearModeRequest();
    
    // State
    void SetCurrentMode(EditorMode mode);
    void SetCartridgeName(const std::string& name);
    
private:
    // UI State
    EditorMode currentMode;
    EditorMode requestedMode;
    std::string cartridgeName;
    
    // UI Layout
    static constexpr int HEIGHT = 18;
    static constexpr int BUTTON_SIZE = 16;
    static constexpr int BUTTON_SPACING = 4;
    
    // Sections
    void RenderLogo(AestheticLayer& renderer);
    void RenderFileButtons(AestheticLayer& renderer);
    void RenderEditorButtons(AestheticLayer& renderer);
    void RenderUtilityButtons(AestheticLayer& renderer);
    
    // Input
    void HandleClick(int mouseX, int mouseY);
    void HandleHotkeys(InputManager& input);
    
    SystemSprites* systemSprites;
};

enum class EditorMode {
    NONE,
    CODE,
    SPRITES,
    MAP,
    MUSIC,
    SFX,
    SETTINGS
};
```

---

## 📋 IMPLEMENTATION PHASES

### Phase 1: Core Infrastructure (45 min)
- [x] Create GlobalTopBar.h
- [x] Create GlobalTopBar.cpp
- [x] Add to CMakeLists.txt
- [x] Define EditorMode enum
- [x] Implement basic structure

### Phase 2: Render System (30 min)
- [ ] RenderLogo (ULICS text)
- [ ] RenderFileButtons (Save, Load, Play, Stop)
- [ ] RenderEditorButtons (Code, Sprites, Map, etc.)
- [ ] RenderUtilityButtons (Settings, Help)
- [ ] Highlight current active editor

### Phase 3: Input Handling (30 min)
- [ ] Click detection for all buttons
- [ ] Hotkey support (F1-F6, F12)
- [ ] Mode switching logic
- [ ] Visual feedback (hover, press)

### Phase 4: Integration (30 min)
- [ ] Add to Engine class
- [ ] Update Engine mode switching
- [ ] Connect to existing editors
- [ ] Remove old F-key handlers from editors

### Phase 5: New Icons (30 min)
- [ ] Design CODE icon (</>)
- [ ] Design MAP icon (grid)
- [ ] Design MUSIC icon (notes)
- [ ] Design SFX icon (waveform)
- [ ] Design SETTINGS icon (gear)
- [ ] Design HELP icon (?)
- [ ] Update SystemSprites

---

## 🎨 VISUAL DESIGN

### Color Scheme (PICO-8)
```cpp
TOPBAR_BG         = COLOR_DARK_BLUE (index 1)
BUTTON_NORMAL     = COLOR_DARK_GRAY (index 5)
BUTTON_HOVER      = COLOR_GRAY (index 6)
BUTTON_ACTIVE     = COLOR_YELLOW (index 10)
ICON_COLOR        = COLOR_WHITE (index 7)
TEXT_COLOR        = COLOR_WHITE (index 7)
```

### Layout Sections

```
┌─────────────────────────────────────────────────────────────┐
│ SECTION 1  │  SECTION 2     │  SECTION 3               │ S4 │
│ Logo+Info  │  File Actions  │  Editor Tabs             │Hlp │
├────────────┼────────────────┼──────────────────────────┼────┤
│ ULICS      │ [💾] [📁]     │ [</>] [🎨] [🗺️] ...      │[?] │
│ cart_name  │ [▶️] [⏹️]     │                           │    │
└────────────┴────────────────┴──────────────────────────┴────┘
 80px         4×(16+4)=80px    6×(16+4)=120px            20px
```

**Total Width:** Flexible (scales to window)  
**Height:** 18px fixed

---

## 🔧 TECHNICAL DETAILS

### File Structure
```
src/
  ui/
    GlobalTopBar.h         (NEW)
    GlobalTopBar.cpp       (NEW)
    SystemSprites.h        (UPDATE - add new icons)
    SystemSprites.cpp      (UPDATE - generate new icons)
  core/
    Engine.h               (UPDATE - add topbar)
    Engine.cpp             (UPDATE - integrate topbar)
CMakeLists.txt             (UPDATE)
```

### Integration with Engine

```cpp
class Engine {
private:
    GlobalTopBar* topBar;
    
    void Update() {
        topBar->Update(*inputManager);
        
        // Check for mode switch request
        EditorMode requested = topBar->GetRequestedMode();
        if (requested != EditorMode::NONE) {
            SwitchMode(requested);
            topBar->ClearModeRequest();
        }
        
        // Update current editor...
    }
    
    void Render() {
        // Clear screen
        // Render current editor (offset Y by topbar height)
        // Render topbar (LAST - always on top)
        topBar->Render(*aestheticLayer);
    }
};
```

### Y-Offset for Editors

All editors need to render below the topbar:

```cpp
// Old: Canvas at Y=0
const int CANVAS_Y = 0;

// New: Canvas at Y=TOPBAR_HEIGHT
const int TOPBAR_HEIGHT = 18;
const int CANVAS_Y = TOPBAR_HEIGHT + 4;  // +4px margin
```

---

## 🎯 FEATURES

### Must Have (MVP)
- [x] Top bar always visible
- [x] Icon-based navigation
- [x] Click to switch editors
- [x] Hotkey support (F1-F6)
- [x] Visual indication of current editor
- [x] Save/Load buttons (delegate to current editor)
- [x] Play/Stop buttons (run cartridge)

### Nice to Have (Future)
- [ ] Hover tooltips ("Code Editor [F1]")
- [ ] Dropdown menus (File, Edit, View, etc.)
- [ ] Breadcrumb navigation
- [ ] Quick search (Ctrl+P)
- [ ] Recent files dropdown
- [ ] Notification badges (unsaved changes)

---

## 📊 EDITOR STATES

### State Machine
```
┌──────────┐  F1   ┌──────────┐
│   NONE   │◄─────►│   CODE   │
└──────────┘       └──────────┘
     │                   │
     │ F2                │ F2
     ▼                   ▼
┌──────────┐       ┌──────────┐
│ SPRITES  │       │ SPRITES  │
└──────────┘       └──────────┘
     │                   │
     │ F3                │ F3
     ▼                   ▼
┌──────────┐       ┌──────────┐
│   MAP    │       │   MAP    │
└──────────┘       └──────────┘
```

**Rules:**
- Only ONE editor active at a time
- Switching saves current editor state
- Engine manages mode transitions
- TopBar just requests mode changes

---

## 🧪 TESTING PLAN

### Manual Tests
- [ ] Click each editor button → Switches mode
- [ ] Press F1-F6 → Switches mode
- [ ] Click Save → Delegates to current editor
- [ ] Click Load → Opens file browser
- [ ] Click Play → Runs cartridge
- [ ] Click Stop → Stops execution
- [ ] Visual highlight shows current editor
- [ ] Topbar doesn't overlap editor content
- [ ] Works at different window sizes

### Edge Cases
- [ ] Switch while editor has unsaved changes
- [ ] Switch while cartridge is running
- [ ] Hotkey conflicts (F5 = Music or Play?)
- [ ] Window too narrow for all buttons
- [ ] Click outside topbar → Ignored

---

## 📝 IMPLEMENTATION CHECKLIST

### Step 1: Create Files
- [ ] src/ui/GlobalTopBar.h
- [ ] src/ui/GlobalTopBar.cpp
- [ ] Update CMakeLists.txt

### Step 2: Define Enums & Constants
- [ ] EditorMode enum
- [ ] Layout constants
- [ ] Icon ID mapping

### Step 3: Implement Rendering
- [ ] Basic topbar background
- [ ] Logo/title rendering
- [ ] Button backgrounds
- [ ] Icon rendering
- [ ] Active state highlighting

### Step 4: Implement Input
- [ ] Click detection
- [ ] Hotkey detection
- [ ] Mode request system

### Step 5: Integrate with Engine
- [ ] Add topBar member
- [ ] Initialize in Engine::Initialize()
- [ ] Update in Engine::Update()
- [ ] Render in Engine::Render()
- [ ] Handle mode switches

### Step 6: Update Editors
- [ ] Offset Code Editor Y positions
- [ ] Offset Sprite Editor Y positions
- [ ] Remove old F-key handlers
- [ ] Test all layouts

### Step 7: Create New Icons
- [ ] CODE icon generation
- [ ] MAP icon generation
- [ ] MUSIC icon generation
- [ ] SFX icon generation
- [ ] SETTINGS icon generation
- [ ] HELP icon generation
- [ ] Regenerate system_sprites.png

---

## 🚀 FUTURE ENHANCEMENTS

### Dynamic Topbar
- Context-sensitive buttons (show tool options for current editor)
- Collapsible sections
- Custom user layouts

### Advanced Features
- Multi-tab support (multiple files open)
- Split view (2 editors side-by-side)
- Floating panels
- Customizable hotkeys

### Professional Polish
- Smooth transitions between modes
- Fade in/out animations
- Button press effects
- Sound feedback

---

## 📐 MOCKUP

```
┌──────────────────────────────────────────────────────────────┐
│ ULICS v1.0 │ my_game.p8 │ 💾 📁 ▶️ ⏹️ │ </> 🎨 🗺️ 🎵 🔊 ⚙️ │ ? │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  [CURRENT EDITOR CONTENT - CODE/SPRITES/MAP/etc.]           │
│                                                              │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

**Active State Example (Sprites active):**
```
┌──────────────────────────────────────────────────────────────┐
│ ULICS │ 💾 📁 ▶️ ⏹️ │ </> [🎨] 🗺️ 🎵 🔊 ⚙️ │ ?                │
└──────────────────────────────────────────────────────────────┘
            Gray    [YELLOW]  Gray...
                    ^^^^^^^^
                    Active!
```

---

## ⏱️ TIME ESTIMATES

| Phase | Est. Time | Running Total |
|-------|-----------|---------------|
| Phase 1: Infrastructure | 45 min | 45 min |
| Phase 2: Rendering | 30 min | 1h 15min |
| Phase 3: Input | 30 min | 1h 45min |
| Phase 4: Integration | 30 min | 2h 15min |
| Phase 5: New Icons | 30 min | 2h 45min |
| Testing & Polish | 15 min | 3h |

**Total:** ~3 hours

---

## 🎓 DESIGN PRINCIPLES

1. **Consistency:** Same look/feel as Sprite Editor toolbar
2. **Simplicity:** Icons over text, minimal clutter
3. **Accessibility:** All features accessible via mouse AND keyboard
4. **Performance:** Render only when needed (dirty flag)
5. **Extensibility:** Easy to add new editors/buttons

---

## ✅ SUCCESS CRITERIA

- [ ] Topbar renders correctly at all window sizes
- [ ] All editor navigation works (click + hotkeys)
- [ ] Visual feedback is clear and immediate
- [ ] No Z-fighting or overlap with editor content
- [ ] Code is clean and well-documented
- [ ] Performance impact < 1ms per frame

---

*Plan created: 2025-12-19 00:33*  
*Ready for implementation*
