# ULICS Menu System - How Cartridge Loading Works

**Reference Document for Future Development**

---

## 📋 Quick Reference

**Where is the menu?**
- Location: `src/scripting/SystemScripts.h`
- It's a Lua script embedded as a C++ string constant: `SystemScripts::MENU_SCRIPT`

**How does it work?**
1. Engine starts → loads `MENU_SCRIPT` (line 189 of `src/core/Engine.cpp`)
2. Menu displays cartridge list using `list_cartridges()` Lua API
3. User selects cart → presses Z/X
4. Menu calls `load_cartridge(cart.path)` (line 102 of SystemScripts.h)
5. Engine loads and **immediately runs** the cartridge

---

## 🗂️ File Structure

### System Menu Script
- **File:** `src/scripting/SystemScripts.h`
- **Constant:** `SystemScripts::MENU_SCRIPT` (lines 29-169)
- **Type:** Embedded Lua script (R"(...)" raw string literal)

### Engine Integration
- **File:** `src/core/Engine.cpp`
- **Load Function:** `Engine::LoadCartridge(path)` (line 443)
- **Menu Loading:** Line 189 - `scriptingManager->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str())`

### Lua API Functions
- **File:** `src/scripting/ScriptingManager.cpp`
- **Relevant Functions:**
  - `load_cartridge(name)` → Loads and runs a cartridge
  - `list_cartridges()` → Returns array of available carts
  - `goto_menu()` → Returns to menu
  - `reset()` → Restarts current cartridge

---

## 🔍 How the Menu Works (Step by Step)

### 1. Engine Startup
```cpp
// src/core/Engine.cpp:188
if (cartridgePath.empty()) {
    SetState(EngineState::MENU);
    scriptLoaded = scriptingManager->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
}
```

### 2. Menu Initialization
```lua
-- src/scripting/SystemScripts.h:56
function _init()
    cartridges = list_cartridges()  -- C++ function
end
```

### 3. User Navigation
```lua
-- Lines 68-95: UP/DOWN arrow keys to select
-- Uses delayed input to prevent rapid scrolling
```

### 4. Cartridge Selection (THE IMPORTANT PART)
```lua
-- Lines 97-106
if (btn(4) or btn(5)) then  -- Z or X button
    if #cartridges > 0 then
        local cart = cartridges[selected_index]
        if cart.path and cart.path ~= "" then
            load_cartridge(cart.path)  -- ← CURRENTLY RUNS IMMEDIATELY
        end
    end
end
```

### 5. Cartridge Loading
```cpp
// src/scripting/ScriptingManager.cpp
int ScriptingManager::Lua_LoadCartridge(lua_State* L) {
    const char* cartName = lua_tostring(L, 1);
    // Calls Engine::LoadCartridge(cartName)
    // Which then RUNS the cartridge
}
```

---

## 🎯 Current Flow Diagram

```
User starts ULICS
    ↓
Engine::Init()
    ↓
No cartridge specified?
    ↓ YES
Engine loads MENU_SCRIPT
    ↓
Menu displays cartridge list
    ↓
User presses ↑/↓ to navigate
    ↓
User presses Z/X to select
    ↓
Menu calls: load_cartridge(cart.path)
    ↓
Engine::LoadCartridge(path)
    ↓
ScriptingManager->LoadAndRunScript(cart.main.lua)
    ↓
GAME RUNS IMMEDIATELY ← Problem!
```

---

## 🔧 What We Need to Change (Task 2.0.5.6)

### Problem
When you select a cartridge from the menu, it **immediately runs the game**. Users want to **inspect/edit code first**.

### Solution
Instead of running the game, **open the Code Editor** with the cartridge loaded.

### New Flow
```
User selects cartridge
    ↓
Menu calls: open_code_editor(cart.path)  ← NEW FUNCTION
    ↓
Engine::LoadCartridge(path)  ← Still loads cart
    ↓
Engine sets activeLayer = F1_CODE_EDITOR  ← Opens editor instead of running
    ↓
Code Editor opens with cart's main.lua
    ↓
User can edit OR press Ctrl+R to run
```

---

## 📝 Implementation Checklist

### Step 1: Create Lua Function `open_code_editor(path)`
- **File:** `src/scripting/ScriptingManager.h`
- Add declaration: `static int Lua_OpenCodeEditor(lua_State* L);`

### Step 2: Implement the Function
- **File:** `src/scripting/ScriptingManager.cpp`
- Implement `Lua_OpenCodeEditor()`:
  ```cpp
  int ScriptingManager::Lua_OpenCodeEditor(lua_State* L) {
      const char* cartPath = lua_tostring(L, 1);
      // Load cart
      sm->engineInstance->LoadCartridge(cartPath);
      // Switch to Code Editor (F1) instead of running
      sm->engineInstance->SetActiveLayer(LayerType::F1_CODE_EDITOR);
      lua_pushboolean(L, true);
      return 1;
  }
  ```
- Register: `RegisterFunction("open_code_editor", &ScriptingManager::Lua_OpenCodeEditor);`

### Step 3: Update Menu to Use New Function
- **File:** `src/scripting/SystemScripts.h`
- **Line 102:** Change from:
  ```lua
  load_cartridge(cart.path)
  ```
  To:
  ```lua
  open_code_editor(cart.path)
  ```

### Step 4: Verify Ctrl+R Works
- **File:** `src/ui/CodeEditor.cpp`
- Ctrl+R should already call `Engine::ReloadCurrentCartridge()`
- This is documented in `docs/KEY_BINDINGS.md:70`

### Step 5: Update Documentation
- **File:** `docs/LUA_API.md`
- Add entry for `open_code_editor(name)` in System API section

---

## 🎮 Existing Keyboard Shortcuts

From `docs/KEY_BINDINGS.md`:

| Shortcut | Function | Location |
|----------|----------|----------|
| **F1** | Toggle Code Editor | System-wide |
| **Ctrl+S** | Save file | Code Editor |
| **Ctrl+R** | Run/Reload cartridge | Code Editor |
| **Ctrl+L** | Toggle file explorer | Code Editor |
| **ESC** | Exit to game view | Planned |

**Ctrl+R already exists!** It calls `Engine::ReloadCurrentCartridge()` which runs the loaded cart.

---

## 🚨 Common Mistakes to Avoid

1. **Don't search for `src/Engine.cpp`** → It's in `src/core/Engine.cpp`
2. **Don't look for `system/` directory** → Doesn't exist, menu is embedded in C++
3. **Don't search for `menu.lua` file** → It's in SystemScripts.h as a string
4. **Don't forget** that all Lua API functions are in ScriptingManager.cpp
5. **Read docs first!** All this info is in:
   - `docs/LUA_API.md` - Complete API reference
   - `docs/KEY_BINDINGS.md` - Keyboard shortcuts
   - Code comments in SystemScripts.h

---

## 📚 Related Documentation

- `docs/LUA_API.md` - Complete Lua API (40 functions)
- `docs/KEY_BINDINGS.md` - All keyboard shortcuts
- `docs/CARTRIDGE_SPEC.md` - Cartridge structure
- `src/scripting/SystemScripts.h` - Menu source code
- `src/core/Engine.cpp` - Engine lifecycle

---

## ✅ Testing Plan

After implementing:

1. **Start ULICS** without arguments → Menu appears
2. **Select a cartridge** (e.g., api_test) → Code Editor should open ✅ VERIFIED
3. **Verify** main.lua is loaded and visible ✅ VERIFIED
4. **Press Ctrl+R** → Game should run ✅ VERIFIED
5. **Press F1** → Should return to Code Editor ✅ VERIFIED
6. **Repeat** with different cartridges ✅ VERIFIED

---

## 🐛 Bugs Found and Fixed

### Bug #1: goto_menu() Crashes with Invalid Cartridge Error

**Symptom:**
```
Lua: goto_menu() called - returning to system menu...
Engine: Loading cartridge from: 
Engine state transition: RUNNING_CARTRIDGE -> LOADING_CARTRIDGE
Engine: Unloading cartridge: ./cartridges\api_test
Engine: Cartridge unloaded.
Engine state transition: LOADING_CARTRIDGE -> ERROR
=== ENGINE ERROR ===
Invalid cartridge: 
Error: 
===================
```

**Root Cause:**
- `Lua_GotoMenu()` was calling `LoadCartridge("")` with empty path
- `LoadCartridge()` validates the path with `IsValidCartridge()` which fails on empty string
- This caused a crash when trying to return to menu from a running game

**Fix Applied:**
```cpp
// BEFORE (BROKEN):
bool success = sm->engineInstance->LoadCartridge("");  // Empty path fails!

// AFTER (FIXED):
sm->engineInstance->UnloadCartridge();
sm->engineInstance->SetState(Engine::EngineState::MENU);
bool success = sm->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
```

**Files Changed:**
- `src/scripting/ScriptingManager.cpp` - Fixed `Lua_GotoMenu()`
- Added `#include "scripting/SystemScripts.h"` for `MENU_SCRIPT` constant

**Result:** ✅ Can now return to menu from any cartridge without errors

---

### Bug #2: open_code_editor() Runs Game Instead of Opening Editor

**Symptom:**
- Selecting cartridge from menu still runs the game immediately
- Code Editor doesn't open

**Root Cause:**
- `Lua_OpenCodeEditor()` called `LoadCartridge()` which automatically runs the cartridge
- Missing call to `SetMode(Engine::EngineMode::CODE_EDITOR)`

**Fix Applied:**
```cpp
// Load the cartridge (loads files)
bool success = sm->engineInstance->LoadCartridge(std::string(path));

if (success) {
    // Switch to Code Editor mode instead of running the game
    sm->engineInstance->SetMode(Engine::EngineMode::CODE_EDITOR);
    std::cout << "Lua: Switched to CODE_EDITOR mode" << std::endl;
}
```

**Files Changed:**
- `src/scripting/ScriptingManager.cpp` - Added `SetMode()` call in `Lua_OpenCodeEditor()`

**Result:** ✅ Cartridges now open in Code Editor as intended

---

## 📊 Final Implementation Summary

### Files Modified (Total: 4 files)

1. **src/scripting/ScriptingManager.h**
   - Added: `static int Lua_OpenCodeEditor(lua_State* L);`

2. **src/scripting/ScriptingManager.cpp**
   - Added: `#include "scripting/SystemScripts.h"`
   - Implemented: `Lua_OpenCodeEditor()` (20 lines)
   - Fixed: `Lua_GotoMenu()` (15 lines)
   - Registered: `open_code_editor` function

3. **src/scripting/SystemScripts.h**
   - Changed: `load_cartridge()` → `open_code_editor()` in menu selection (line 102)
   - Updated: Controls text "Z/X: LOAD" → "Z/X: OPEN EDITOR" (line 164)

4. **src/core/Engine.cpp** (Debug logs - optional)
   - Added: Debug logging for F1 mode switching

### New Lua API Function

**Function:** `open_code_editor(path)`

**Purpose:** Opens Code Editor with specified cartridge instead of running it

**Parameters:**
- `path` (string) - Path to cartridge directory

**Returns:**
- `boolean` - true if loaded successfully

**Example:**
```lua
-- From menu script
if btnp(4) or btnp(5) then
    open_code_editor(cartridges[selected].path)
end
```

**Implementation:**
```cpp
int ScriptingManager::Lua_OpenCodeEditor(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* path = luaL_checkstring(L, 1);
    
    if (sm && sm->engineInstance) {
        bool success = sm->engineInstance->LoadCartridge(std::string(path));
        if (success) {
            sm->engineInstance->SetMode(Engine::EngineMode::CODE_EDITOR);
        }
        lua_pushboolean(L, success);
        return 1;
    }
    
    lua_pushboolean(L, false);
    return 1;
}
```

---

## 🎯 Workflow Comparison

### Before Implementation

```
User Action          →  Result
──────────────────────────────────────────
Start ULICS          →  Menu appears
Select "api_test"    →  GAME RUNS IMMEDIATELY ❌
Need to edit code    →  Must exit, find file, edit externally
Run modified code    →  Restart ULICS, select cart again
```

### After Implementation

```
User Action          →  Result
──────────────────────────────────────────
Start ULICS          →  Menu appears
Select "api_test"    →  CODE EDITOR OPENS ✅
Can inspect/edit     →  Full editor with syntax highlighting
Press Ctrl+R         →  Run game
Press F1/ESC         →  Toggle between editor and game
Press F1 in game     →  Return to Code Editor
goto_menu() in game  →  Return to main menu ✅
```

---

## 🔧 Technical Details

### Engine State Machine

```
BOOT → MENU → LOADING_CARTRIDGE → RUNNING_CARTRIDGE
                ↑                           ↓
                └───────── goto_menu() ─────┘
```

### Engine Mode System

```
GAME ←──────→ CODE_EDITOR
 ↑  F1/ESC        F1
 │
 └── Default when cartridge loads
```

### Key Keyboard Shortcuts

| Key | From Mode | To Mode | Function |
|-----|-----------|---------|----------|
| F1 | GAME | CODE_EDITOR | Open editor |
| F1 | CODE_EDITOR | GAME | Return to game |
| ESC | CODE_EDITOR | GAME | Return to game |
| ESC | GAME | (no change) | Ignored |
| Ctrl+R | CODE_EDITOR | GAME | Run/reload cart |

---

## 📝 Code Patterns

### Pattern 1: Loading Menu from Game

```cpp
// WRONG - Causes error
sm->engineInstance->LoadCartridge("");

// RIGHT - Proper menu loading
sm->engineInstance->UnloadCartridge();
sm->engineInstance->SetState(Engine::EngineState::MENU);
sm->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
```

### Pattern 2: Opening Editor Instead of Running

```cpp
// Load cartridge
bool success = engine->LoadCartridge(path);

// Switch to editor mode (don't run)
if (success) {
    engine->SetMode(Engine::EngineMode::CODE_EDITOR);
}
```

### Pattern 3: System Script Access

```cpp
// Include header
#include "scripting/SystemScripts.h"

// Use constant
sm->LoadAndRunScript(SystemScripts::MENU_SCRIPT.c_str());
```

---

## 🎓 Lessons Learned

### 1. Engine State vs Engine Mode
- **State:** Boot, Menu, Loading, Running, Error
- **Mode:** Game, CodeEditor, SpriteEditor, etc.
- **Key:** State controls lifecycle, Mode controls active view

### 2. LoadCartridge() Side Effects
- Automatically runs cartridge after loading
- Must call `SetMode()` explicitly to change behavior
- Cannot load empty path - use direct menu script loading

### 3. Lua API Evolution
- `load_cartridge()` - Loads AND runs (original)
- `open_code_editor()` - Loads but opens in editor (new)
- `goto_menu()` - Returns to menu (fixed)

### 4. Debug-First Development
- Added debug logs before fixing bugs
- Logs revealed exact failure points
- Pattern: Always log state transitions

---

## 🚀 Future Enhancements

### Potential Improvements

1. **Save Editor State**
   - Remember cursor position between mode switches
   - Preserve undo history when toggling

2. **Smart Mode Switching**
   - Auto-save before running
   - Compile-check before execution

3. **Multi-File Support**
   - Open multiple files in tabs
   - Switch files without reloading

4. **Quick Actions**
   - Ctrl+Shift+R - Run without switching mode
   - Ctrl+E - Jump to editor from game

---

**Created:** 2025-12-17  
**Updated:** 2025-12-17 (Implementation Complete)  
**Status:** ✅ Production Ready  
**Next Review:** When adding Sprite/Map/SFX editors
