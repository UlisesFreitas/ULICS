# ULICS UI System Documentation

**Version:** 2.0 (In Progress)  
**Status:** Phase 2.0.1-2.0.2 COMPLETED  
**Last Updated:** 2025-12-14

---

## 🎯 **Overview**

ULICS uses a **100% custom pixel-perfect retro UI system** built with native rendering primitives. NO external libraries (ImGui, Nuklear, etc.). This approach ensures authentic fantasy console aesthetics like PICO-8/TIC-80.

### **Philosophy**

- ✅ All UI rendered using `AestheticLayer` primitives (`rectfill`, `print`, `pset`, `line`)
- ✅ Pixel-perfect retro style
- ✅ Consistent with fantasy console aesthetic
- ✅ Zero external dependencies
- ✅ Full control over appearance and behavior

---

## 📦 **Components Status**

| Component | Status | Version | Priority |
|-----------|--------|---------|----------|
| **UISystem** | ✅ **IMPLEMENTED** | 2.0.1 | CRITICAL |
| **CodeEditor** | 🚧 **IN PROGRESS** | 2.0.2 | CRITICAL |
| Sprite Editor | ⏳ Pending | 2.0.6 | HIGH |
| Map Editor | ⏳ Pending | 2.0.7 | HIGH |
| SFX Editor | ⏳ Pending | 2.0.8 | MEDIUM |
| Music Tracker | ⏳ Pending | 2.0.9 | MEDIUM |
| Palette Editor | ⏳ Pending | 2.0.10 | MEDIUM |

**Legend:**
- ✅ **IMPLEMENTED** - Fully functional
- 🚧 **IN PROGRESS** - Partially implemented
- ⏳ **PENDING** - Not started

---

## 🎨 **UISystem (2.0.1) - COMPLETED**

Base UI rendering system using custom primitives.

### **Implemented Features**

#### **Panel Rendering**
```cpp
void RenderPanel(AestheticLayer& layer, int x, int y, int w, int h, 
                 const std::string& title, bool focused = true);
```
- ✅ Window-style panels with title bars
- ✅ 2-pixel retro border
- ✅ Focused/unfocused states (color change)
- ✅ Purple title bar (PICO-8 inspired)
- ✅ Dark blue background

#### **Box Rendering**
```cpp
void RenderBox(AestheticLayer& layer, int x, int y, int w, int h, int color);
```
- ✅ Simple filled rectangles with borders
- ✅ Customizable color

#### **Text Rendering**
```cpp
int RenderText(AestheticLayer& layer, const std::string& text, 
               int x, int y, int maxWidth, int color);
void RenderLine(AestheticLayer& layer, const std::string& text, 
                int x, int y, int color);
void RenderCenteredText(AestheticLayer& layer, const std::string& text,
                        int x, int y, int w, int h, int color);
```
- ✅ Multi-line text with word wrap
- ✅ Single line rendering
- ✅ Centered text in boxes
- ✅ Returns number of lines rendered

#### **Button Rendering**
```cpp
bool RenderButton(AestheticLayer& layer, const std::string& text,
                  int x, int y, int w, int h, bool pressed = false);
```
- ✅ Basic button rendering
- ✅ Pressed/unpressed states
- ✅ Centered text
- ⚠️ **TODO:** Actual hover detection (requires mouse integration)

#### **Utilities**
```cpp
bool IsMouseInRect(int mx, int my, int x, int y, int w, int h);
int GetCharWidth() const;   // Returns 4 pixels
int GetCharHeight() const;  // Returns 6 pixels
int GetTextWidth(const std::string& text) const;
```
- ✅ Mouse collision detection helper
- ✅ Text measurement
- ✅ Character size constants

#### **Color Scheme (PICO-8 Inspired)**
```cpp
static constexpr int COLOR_BACKGROUND = 0;      // Black
static constexpr int COLOR_DARK_BLUE = 1;       // Dark blue
static constexpr int COLOR_DARK_PURPLE = 2;     // Dark purple
static constexpr int COLOR_DARK_GREEN = 3;      // Dark green
static constexpr int COLOR_BROWN = 4;           // Brown
static constexpr int COLOR_DARK_GRAY = 5;       // Dark gray
static constexpr int COLOR_LIGHT_GRAY = 6;      // Light gray
static constexpr int COLOR_WHITE = 7;           // White
static constexpr int COLOR_RED = 8;             // Red
static constexpr int COLOR_ORANGE = 9;          // Orange
static constexpr int COLOR_YELLOW = 10;         // Yellow
static constexpr int COLOR_GREEN = 11;          // Green
static constexpr int COLOR_BLUE = 12;           // Blue
static constexpr int COLOR_INDIGO = 13;         // Indigo (purple)
static constexpr int COLOR_PINK = 14;           // Pink
static constexpr int COLOR_PEACH = 15;          // Peach

// UI Theme colors
static constexpr int COLOR_PANEL_BG = COLOR_DARK_BLUE;
static constexpr int COLOR_PANEL_BORDER = COLOR_LIGHT_GRAY;
static constexpr int COLOR_TITLE_BG = COLOR_INDIGO;
static constexpr int COLOR_TITLE_TEXT = COLOR_WHITE;
static constexpr int COLOR_TEXT = COLOR_WHITE;
static constexpr int COLOR_BUTTON = COLOR_DARK_PURPLE;
static constexpr int COLOR_BUTTON_HOVER = COLOR_INDIGO;
static constexpr int COLOR_BUTTON_PRESSED = COLOR_DARK_GREEN;
```

### **Example Usage**

```cpp
#include "ui/UISystem.h"
#include "rendering/AestheticLayer.h"

UISystem ui;
AestheticLayer layer;

// Render a panel
ui.RenderPanel(layer, 10, 10, 236, 200, "MY PANEL", true);

// Render text
ui.RenderText(layer, "Hello\nWorld!", 20, 30, 200, UISystem::COLOR_WHITE);

// Render a button
bool pressed = false;  // Get from input
ui.RenderButton(layer, "CLICK ME", 50, 180, 80, 20, pressed);

// Measure text
int width = ui.GetTextWidth("Some text");
```

### **Files**
- `src/ui/UISystem.h` - Header
- `src/ui/UISystem.cpp` - Implementation

---

## ✏️ **CodeEditor (2.0.2) - IN PROGRESS**

In-engine Lua code editor. **THE CORE FANTASY CONSOLE FEATURE!**

### **Phase 2.0.2: Basic Text Editing - COMPLETED ✅**
### **Phase 2.0.3: Syntax Highlighting - COMPLETED ✅**
### **Phase 2.0.4: Save/Run Integration - COMPLETED ✅**

#### **Implemented Features**

**Text Buffer:**
- ✅ Multi-line text storage (`std::vector<std::string>`)
- ✅ Load from file (`Load()`)
- ✅ Save to file (`Save()`)
- ✅ Get/Set full text content
- ✅ Modified flag tracking

**Cursor Movement:**
- ✅ Arrow keys (left, right, up, down)
- ✅ Home/End (start/end of line)
- ✅ Page Up/Down (scroll by screen)
- ✅ Auto-scroll to keep cursor visible
- ✅ Cursor clamping to line length

**Text Editing:**
- ✅ Character insertion (SDL_TEXTINPUT working!)
- ✅ Backspace (delete char before cursor)
- ✅ Delete (delete char at cursor)
- ✅ Enter (create new line)
- ✅ Tab (insert 4 spaces for indentation)
- ✅ Line merging on backspace/delete

**Syntax Highlighting:**
- ✅ Keywords (if, function, for, etc.) - Blue
- ✅ Built-in functions (cls, print, btn, etc.) - Green
- ✅ Strings ("text", 'text') - Pink
- ✅ Numbers (123, 0.5, -10) - Orange
- ✅ Comments (-- ...) - Gray
- ✅ Operators (+, -, *, =, etc.) - Peach
- ✅ Token-based parsing (simple but effective)

**Hotkeys:**
- ✅ **Ctrl+S** - Save file (shows "SAVED!" message)
- ✅ **Ctrl+R** - Run/Reload (saves & triggers hot reload)
- ✅ **Tab** - Insert 4 spaces
- ✅ Modifier key detection (Ctrl, Shift)

**Rendering:**
- ✅ Title bar ("CODE EDITOR")
- ✅ Line numbers (3 digits, gray background)
- ✅ Syntax-highlighted text
- ✅ Cursor rendering (yellow blinking box)
- ✅ Status bar (line:col, filename, SAVED!/MODIFIED)
- ✅ Visible area: 28 lines × 58 columns

**Display Constants:**
```cpp
static constexpr int VISIBLE_LINES = 28;  // ~224 pixels / 8 pixels per line
static constexpr int VISIBLE_COLS = 58;   // ~232 pixels / 4 pixels per char
```

#### **Controls (Implemented)**

| Key | Action |
|-----|--------|
| ⬅️➡️⬆️⬇️ | Move cursor |
| Home | Start of line |
| End | End of line |
| Page Up | Scroll up one page |
| Page Down | Scroll down one page |
| Backspace | Delete char before cursor |
| Delete | Delete char at cursor |
| Enter | New line |
| Tab | Insert 4 spaces |
| **Ctrl+S** | Save file |
| **Ctrl+R** | Run/Reload |
| A-Z, 0-9, symbols | Type characters |

### **Phase 2.0.5: Mode Switcher - PENDING ⏳**

**Text Buffer:**
- ✅ Multi-line text storage (`std::vector<std::string>`)
- ✅ Load from file (`Load()`)
- ✅ Save to file (`Save()`)
- ✅ Get/Set full text content
- ✅ Modified flag tracking

**Cursor Movement:**
- ✅ Arrow keys (left, right, up, down)
- ✅ Home/End (start/end of line)
- ✅ Page Up/Down (scroll by screen)
- ✅ Auto-scroll to keep cursor visible
- ✅ Cursor clamping to line length

**Text Editing:**
- ✅ Backspace (delete char before cursor)
- ✅ Delete (delete char at cursor)
- ✅ Enter (create new line)
- ✅ Line merging on backspace/delete
- ⚠️ **Character insertion** - NOT WORKING YET!
  - Need SDL_TEXTINPUT event handling
  - `InsertChar()` method exists but not called

**Rendering:**
- ✅ Title bar ("CODE EDITOR")
- ✅ Line numbers (3 digits, gray background)
- ✅ Text area with scrolling
- ✅ Cursor rendering (yellow blinking box)
- ✅ Status bar (line:col, filename, modified status)
- ✅ Visible area: 28 lines × 58 columns

**Display Constants:**
```cpp
static constexpr int VISIBLE_LINES = 28;  // ~224 pixels / 8 pixels per line
static constexpr int VISIBLE_COLS = 58;   // ~232 pixels / 4 pixels per char
```

#### **Example Usage**

```cpp
#include "ui/CodeEditor.h"
#include "input/InputManager.h"

CodeEditor editor;

// Initialize and load file
editor.Initialize("cartridges/mygame/main.lua");

// In game loop:
editor.Update(input);  // Handle keyboard input
editor.Render(layer, ui);  // Draw editor

// Save file
if (needsSave) {
    editor.Save();
}
```

#### **Controls (Implemented)**

| Key | Action |
|-----|--------|
| ⬅️➡️⬆️⬇️ | Move cursor |
| Home | Start of line |
| End | End of line |
| Page Up | Scroll up one page |
| Page Down | Scroll down one page |
| Backspace | Delete char before cursor |
| Delete | Delete char at cursor |
| Enter | New line |

### **Phase 2.0.3: Syntax Highlighting - PENDING ⏳**

#### **Planned Features**

**Syntax Highlighting:**
- ⏳ Keyword highlighting (function, if, for, while, etc.) - Cyan
- ⏳ String highlighting ("text", 'text') - Pink
- ⏳ Number highlighting (123, 0.5) - Orange
- ⏳ Comment highlighting (-- comment) - Gray
- ⏳ Simple token-based parsing (regex or string matching)

**Files to create:**
- `src/ui/LuaSyntax.h` - Lua syntax definitions
- Update `CodeEditor::Render()` to apply syntax colors

### **Phase 2.0.4: Save/Run Integration - PENDING ⏳**

#### **Planned Features**

**Hotkeys:**
- ⏳ Ctrl+S - Save file
- ⏳ Ctrl+R - Run/reload cartridge
- ⏳ Ctrl+C - Copy selection
- ⏳ Ctrl+V - Paste
- ⏳ Tab - Insert 4 spaces (indentation)
- ⏳ Shift+Tab - Unindent
- ⏳ Escape - Toggle to game view

**Text Input:**
- ⏳ SDL_TEXTINPUT event handling in Engine
- ⏳ Call `editor.InsertChar()` for each character
- ⏳ Support for all ASCII printable characters

**Integration with Engine:**
- ⏳ Mode switcher (CODE_EDITOR vs GAME_RUNNING)
- ⏳ Hot reload on Ctrl+R
- ⏳ Seamless switching between editor and game

### **Files**
- `src/ui/CodeEditor.h` - Header (DONE)
- `src/ui/CodeEditor.cpp` - Implementation (PARTIAL)

---

## 🚧 **Current Limitations**

### **CodeEditor**
1. **NO TEXT INPUT YET** - Cannot type characters!
   - Reason: SDL_TEXTINPUT events not captured
   - Fix: Add event handling in Engine
   - Impact: Editor is view-only currently
   
2. **NO SYNTAX HIGHLIGHTING** - All text is white
   - Planned in Phase 2.0.3
   
3. **NO SAVE/RUN HOTKEYS** - Manual save required
   - Planned in Phase 2.0.4

4. **BASIC CURSOR** - Doesn't blink based on time
   - Current: Frame-based blinking (mod 60)
   - Better: Time-based (500ms intervals)

5. **NO SELECTION** - Cannot select/copy/paste
   - Planned in Phase 2.0.4

6. **NO UNDO/REDO** - Destructive edits
   - Planned for future

### **UISystem**
1. **NO MOUSE HOVER DETECTION** - Buttons don't react to mouse
   - `IsMouseInRect()` exists but not used
   - Need mouse integration

2. **NO SCROLLBARS** - Scroll position not visualized
   - Could add in future

---

## 📊 **Roadmap**

### **COMPLETED ✅**
- [x] 2.0.1 - UISystem Base (2-3h)
- [x] 2.0.2 - CodeEditor Part 1: Basic editing (3-4h)

### **NEXT IMMEDIATE (Saturday Night!)** 🚀
- [ ] **FIX TEXT INPUT** - Add SDL_TEXTINPUT handling (10 minutes)
  - This is CRITICAL - without it, editor is useless
- [ ] 2.0.3 - CodeEditor Part 2: Syntax highlighting (2-3h)
- [ ] 2.0.4 - CodeEditor Part 3: Save/Run hotkeys (1-2h)

### **UPCOMING**
- [ ] 2.0.5 - Mode Switcher (F1=Code, Esc=Game) (1-2h)
- [ ] 2.0.6 - Sprite Editor (4-5h)
- [ ] 2.0.7 - Map Editor (4-5h)
- [ ] 2.0.8 - SFX Editor (3-4h)
- [ ] 2.0.9 - Music Tracker (4-5h)
- [ ] 2.0.10 - Palette Editor (2-3h)

### **FUTURE**
- [ ] Copy/paste (Ctrl+C/V)
- [ ] Undo/redo
- [ ] Find/replace
- [ ] Code completion
- [ ] Error indicators
- [ ] Breakpoint debugging

---

## 🎮 **Integration with Engine**

### **Current State**
- UISystem and CodeEditor are **standalone classes**
- Not yet integrated into Engine main loop
- No mode switching implemented

### **Planned Integration**

```cpp
// Engine.h
enum class EngineMode {
    CODE_EDITOR,     // F1 - Edit code
    SPRITE_EDITOR,   // F2 - Edit sprites
    MAP_EDITOR,      // F3 - Edit map
    SFX_EDITOR,      // F4 - Edit SFX
    GAME_RUNNING     // Esc - Play game
};

class Engine {
private:
    EngineMode currentMode;
    std::unique_ptr<CodeEditor> codeEditor;
    std::unique_ptr<UISystem> uiSystem;
    // ... other editors
};
```

```cpp
// Engine::Run() - Pseudo-code
void Engine::Run() {
    while (isRunning) {
        // Poll events
        SDL_PollEvent(&event);
        
        // Check mode switch
        if (event.key == F1) currentMode = CODE_EDITOR;
        if (event.key == ESC) currentMode = GAME_RUNNING;
        
        // Update based on mode
        switch (currentMode) {
            case CODE_EDITOR:
                codeEditor->Update(inputManager);
                codeEditor->Render(aestheticLayer, uiSystem);
                break;
            case GAME_RUNNING:
                // Current game logic
                activeGame->_update();
                activeGame->_draw();
                break;
        }
        
        // Render
        aestheticLayer.Present();
    }
}
```

---

## 📝 **Developer Notes**

### **Design Decisions**

1. **Why custom UI instead of ImGui?**
   - **Authenticity:** PICO-8/TIC-80 don't use ImGui
   - **Aesthetics:** Full control over retro pixel-perfect look
   - **Consistency:** Same rendering as game (same primitives)
   - **Learning:** More work but better understanding

2. **Why std::vector<std::string> for text buffer?**
   - **Simplicity:** Easy to understand and debug
   - **Good enough:** Fantasy console files are small
   - **Future:** Can optimize if needed (gap buffer, rope)

3. **Why no rich text editing features?**
   - **Scope:** Not needed for fantasy console
   - **Focus:** Simple and functional, not IDE-level
   - **PICO-8 philosophy:** Basic but sufficient

### **Performance Considerations**

- ✅ Text rendering is fast (< 1ms for full screen)
- ✅ Line numbers pre-calculated
- ✅ Only visible lines rendered
- ⚠️ Word wrap re-calculates every frame (could cache)
- ⚠️ No text buffer optimization (fine for < 10k lines)

### **Known Issues**

1. **Cursor blinking** - Uses frame count, not time
2. **No text input** - SDL_TEXTINPUT not implemented
3. **Line wrap in editor** - Lines truncate, not wrap
4. **No horizontal scroll** - Long lines get cut off

---

## 🔗 **Related Documentation**

- `docs/LUA_API.md` - Lua API reference
- `docs/KEYBOARD_AND_CONTROLLER.md` - Input reference
- `plan-ulics-2.json` - Full Phase 2.0 roadmap
- `README.md` - Main project documentation

---

## 📊 **Statistics**

**Total Lines of Code (UI System):**
- UISystem.h: ~160 lines
- UISystem.cpp: ~170 lines
- CodeEditor.h: ~170 lines
- CodeEditor.cpp: ~420 lines
- **Total: ~920 lines**

**Time Invested:**
- 2.0.1 UISystem: ~2.5 hours
- 2.0.2 CodeEditor Part 1: ~3 hours
- **Total so far: ~5.5 hours**

**Estimated Completion:**
- Remaining work: ~15-20 hours
- Full IDE: ~25-30 hours total

---

**Last Updated:** 2025-12-14 01:20 AM  
**Status:** Phase 2.0.2 completed, ready for 2.0.3  
**Next Task:** Add SDL_TEXTINPUT support, then syntax highlighting
