# Code Editor - File Explorer Sidebar

**Task ID:** 2.0.5.2  
**Status:** 📋 PENDING  
**Priority:** HIGH  
**Estimated Time:** 2-3 hours  
**Created:** 2025-12-14

---

## 🎯 **Objective**

Add a **file explorer sidebar** to the Code Editor that allows users to browse and switch between `.lua` and `.json` files within the current cartridge directory.

### **User Story:**
> _As a developer, I want to browse all files in my cartridge from within the Code Editor, so I can easily switch between `main.lua`, `config.json`, and other Lua modules without leaving the editor._

---

## 📋 **Requirements**

### **Core Features:**

1. **Sidebar Toggle**
   - ✅ Keyboard shortcut: `Ctrl+L` (toggle visibility)
   - 🔮 Future: Visual icon button for toggle
   - Default state: Hidden

2. **File Scanning**
   - Scan current cartridge directory (`cartridges/<name>/`)
   - Recursive scanning for subdirectories
   - Filter: Show only `.lua` and `.json` files
   - Use `std::filesystem::directory_iterator`

3. **Tree View Display**
   - Custom UI rendering (no ImGui)
   - File icons: `L` for .lua, `J` for .json
   - Indentation for nested directories (8px per level)
   - Current file highlighted
   - Scrollable list if files > visible area

4. **File Switching**
   - Click on file → load in editor
   - Keyboard navigation:
     - `↑`/`↓` arrows to navigate selection
     - `Enter` to open selected file
   - Preserve current file state before switching
   - Auto-save option (ask or auto?)

5. **Visual Design**
   - Sidebar width: 60px
   - Position: Left side of editor
   - Colors (from UISystem palette):
     - Background: `COLOR_DARK_GRAY`
     - Selected file: `COLOR_INDIGO`
     - Text: `COLOR_WHITE`
     - File icons: `COLOR_YELLOW`

---

## 🏗️ **Architecture**

### **Component Structure:**

```
CodeEditor
├── FileExplorer (new class)
│   ├── ScanDirectory()
│   ├── RenderSidebar()
│   ├── HandleInput()
│   └── GetSelectedFile()
└── Integration
    ├── Toggle sidebar (Ctrl+L)
    ├── Adjust editor layout when sidebar visible
    └── Load selected file
```

### **New Classes:**

#### **FileExplorer.h / FileExplorer.cpp**

```cpp
#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <string>
#include <vector>
#include <filesystem>

class AestheticLayer;
class InputManager;

struct FileEntry {
    std::string name;
    std::string fullPath;
    bool isDirectory;
    int depth;  // For indentation
    std::string icon;  // "L" or "J"
};

class FileExplorer {
public:
    FileExplorer();
    ~FileExplorer();

    // Scan directory
    void ScanDirectory(const std::string& cartridgePath);
    
    // Rendering
    void Render(AestheticLayer& layer, int x, int y, int width, int height);
    
    // Input handling
    void HandleInput(InputManager& input);
    
    // Selection
    std::string GetSelectedFile() const;
    void SetCurrentFile(const std::string& filepath);
    
    // State
    bool IsVisible() const { return visible; }
    void SetVisible(bool vis) { visible = vis; }
    void Toggle() { visible = !visible; }

private:
    std::vector<FileEntry> files;
    int selectedIndex;
    int scrollOffset;
    bool visible;
    std::string currentFile;
    
    // Helper functions
    void ScanRecursive(const std::filesystem::path& path, int depth);
    bool IsLuaOrJson(const std::string& filename) const;
    std::string GetFileIcon(const std::string& extension) const;
};

#endif // FILEEXPLORER_H
```

### **CodeEditor Integration:**

**Modified members:**
```cpp
class CodeEditor {
private:
    std::unique_ptr<FileExplorer> fileExplorer;  // NEW
    bool sidebarVisible;  // NEW
    std::string cartridgePath;  // NEW - track current cartridge
    
    // Adjust these based on sidebar visibility
    int editorOffsetX;  // 0 or 60 depending on sidebar
    int editorWidth;    // Adjust for sidebar
};
```

**Modified methods:**
```cpp
// In CodeEditor::Initialize()
void CodeEditor::Initialize(const std::string& filename) {
    // Extract cartridge path from filename
    // e.g., "cartridges/mygame/main.lua" → "cartridges/mygame"
    cartridgePath = ExtractCartridgePath(filename);
    
    // Initialize file explorer
    fileExplorer = std::make_unique<FileExplorer>();
    fileExplorer->ScanDirectory(cartridgePath);
    fileExplorer->SetVisible(false);  // Hidden by default
}

// In CodeEditor::Update()
void CodeEditor::Update(InputManager& input) {
    // Ctrl+L to toggle sidebar
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_L)) {
        fileExplorer->Toggle();
        AdjustEditorLayout();
    }
    
    // If sidebar visible, handle its input
    if (fileExplorer->IsVisible()) {
        fileExplorer->HandleInput(input);
        
        // Check if file selected
        std::string selectedFile = fileExplorer->GetSelectedFile();
        if (!selectedFile.empty() && selectedFile != currentFilename) {
            SwitchToFile(selectedFile);
        }
    }
    
    // Rest of editor update...
}

// In CodeEditor::Render()
void CodeEditor::Render(AestheticLayer& layer, UISystem& ui) {
    // Render sidebar if visible
    if (fileExplorer->IsVisible()) {
        fileExplorer->Render(layer, 0, EDITOR_TOP, 60, EDITOR_HEIGHT);
    }
    
    // Render editor content (offset if sidebar visible)
    int editorX = fileExplorer->IsVisible() ? 60 : 0;
    // ... rest of rendering with adjusted X offset
}
```

---

## 🎨 **UI Layout**

### **Sidebar Visible (60px wide):**

```
┌──────────────────────────────────────────────┐
│ CODE                                         │ ← Title bar (10px)
├──────┬───────────────────────────────────────┤
│ L ma │   1  function _init()                 │
│ J co │   2      -- Initialize game           │
│ L ut │   3  end                               │
│      │   4                                    │
│      │   5  function _update()                │
│      │   ...                                  │
│      │                                        │
│ 60px │  196px editor area                    │
│      │                                        │
├──────┴───────────────────────────────────────┤
│ Ln12:34 main.lua                   MODIFIED │ ← Status bar (10px)
└──────────────────────────────────────────────┘
```

### **Sidebar Hidden (full width):**

```
┌──────────────────────────────────────────────┐
│ CODE                                         │
├──────────────────────────────────────────────┤
│   1  function _init()                        │
│   2      -- Initialize game                  │
│   3  end                                     │
│   ...                                        │
│                                              │
│  256px full editor area                     │
│                                              │
├──────────────────────────────────────────────┤
│ Ln12:34 main.lua                   MODIFIED │
└──────────────────────────────────────────────┘
```

### **Sidebar Detail View:**

```
┌──────┐
│ L ma │ ← main.lua (selected = INDIGO background)
│ J co │ ← config.json
│ L he │ ← helpers.lua
│ L pl │ ← player.lua
│      │
│▲    │ ← Scroll up indicator (if needed)
│▼    │ ← Scroll down indicator
└──────┘
```

---

## 🔧 **Implementation Plan**

### **Phase 1: File Explorer Class (45 min)**

1. Create `src/ui/FileExplorer.h` and `.cpp`
2. Implement `ScanDirectory()` with `std::filesystem`
3. Build `FileEntry` vector with proper depth tracking
4. Implement basic rendering (file list with icons)

### **Phase 2: CodeEditor Integration (45 min)**

1. Add `FileExplorer` member to `CodeEditor`
2. Implement `Ctrl+L` toggle in `Update()`
3. Adjust editor layout calculations
4. Update `Render()` to show sidebar

### **Phase 3: File Switching (30 min)**

1. Implement `SwitchToFile()` method
2. Handle file loading/unloading
3. Preserve cursor position when switching
4. Update file explorer's current file highlight

### **Phase 4: Input Handling (30 min)**

1. Arrow key navigation in file list
2. Enter to select file
3. Scroll handling if files > visible area
4. Mouse click support (optional)

### **Phase 5: Polish & Testing (30 min)**

1. Test with various cartridge structures
2. Handle edge cases (empty directories, no files)
3. Update `docs/KEY_BINDINGS.md`
4. Visual polish (colors, spacing)

---

## ⌨️ **Keyboard Shortcuts**

| Key | Action |
|-----|--------|
| **Ctrl+L** | Toggle sidebar visibility |
| **↑** | Navigate up in file list (when sidebar focused) |
| **↓** | Navigate down in file list |
| **Enter** | Open selected file |
| **Esc** | Close sidebar (same as Ctrl+L) |

**TODO:** Update `docs/KEY_BINDINGS.md` with these bindings!

---

## 🧪 **Testing Checklist**

- [ ] Sidebar toggles with `Ctrl+L`
- [ ] Files are scanned correctly from cartridge directory
- [ ] Only `.lua` and `.json` files are shown
- [ ] Subdirectories are shown with proper indentation
- [ ] Current file is highlighted
- [ ] Clicking file loads it in editor
- [ ] Arrow keys navigate file list
- [ ] Enter opens selected file
- [ ] Editor layout adjusts correctly when sidebar visible/hidden
- [ ] File changes are preserved when switching
- [ ] Scroll works with many files
- [ ] Works with empty directories
- [ ] Works with deeply nested files

---

## 🐛 **Edge Cases**

1. **No files in cartridge** → Show message "No files found"
2. **Current file not in list** → Still highlight it somehow
3. **File deleted externally** → Refresh file list (auto or manual?)
4. **New file created** → Need "Refresh" function (Ctrl+Shift+R?)
5. **Very long filenames** → Truncate with ellipsis
6. **Many files (>20)** → Scrolling required

---

## 🔮 **Future Enhancements**

- [ ] File creation (right-click → New File)
- [ ] File deletion (with confirmation)
- [ ] File renaming
- [ ] Folder creation
- [ ] Drag-and-drop file import
- [ ] Search/filter files
- [ ] Right-click context menu
- [ ] Icons for different file types
- [ ] Show file size/date modified
- [ ] Collapse/expand directories

---

## 📖 **Related Documentation**

- `docs/UI_SYSTEM.md` - UI system overview
- `docs/KEY_BINDINGS.md` - ⚠️ **TODO: UPDATE WITH CTRL+L**
- `plan-ulics-2.json` - Task 2.0.5.2

---

## 📊 **Status**

| Component | Status |
|-----------|--------|
| FileExplorer class | ⏳ Pending |
| Directory scanning | ⏳ Pending |
| Sidebar rendering | ⏳ Pending |
| CodeEditor integration | ⏳ Pending |
| File switching | ⏳ Pending |
| Keyboard shortcuts | ⏳ Pending |
| Documentation update | ⏳ Pending |

---

**Created:** 2025-12-14  
**Estimated:** 2-3 hours  
**Priority:** HIGH  
**Depends on:** 2.0.1 (UISystem), 2.0.2-2.0.4 (CodeEditor)
