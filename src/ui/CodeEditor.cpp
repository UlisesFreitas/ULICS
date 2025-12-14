#include "ui/CodeEditor.h"
#include "ui/UISystem.h"
#include "ui/LuaSyntax.h"
#include "ui/FileExplorer.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "input/InputConstants.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>  // For debug output

CodeEditor::CodeEditor()
    : cursorLine(0), cursorCol(0), scrollY(0), scrollX(0), modified(false), savedMessageTimer(0),
      reloadedMessageTimer(0), fileWatchingEnabled(true),
      scrollbarDragging(false), scrollbarDragOffset(0),
      keyRepeatDelay(20), keyRepeatInterval(3),
      leftKeyHoldFrames(0), rightKeyHoldFrames(0), upKeyHoldFrames(0), downKeyHoldFrames(0) {
    // Start with empty file
    lines.push_back("");
}

CodeEditor::~CodeEditor() {
}

// ============================================
// CORE FUNCTIONS
// ============================================

bool CodeEditor::Initialize(const std::string& filename) {
    currentFilename = filename;
    
    // Extract cartridge path from filename (Phase 2.0.5.2)
    // e.g., "cartridges/mygame/main.lua" → "cartridges/mygame"
    if (!filename.empty()) {
        std::filesystem::path filepath(filename);
        cartridgePath = filepath.parent_path().string();
    }
    
    // Initialize file explorer (Phase 2.0.5.2)
    fileExplorer = std::make_unique<FileExplorer>();
    if (!cartridgePath.empty()) {
        fileExplorer->ScanDirectory(cartridgePath);
        fileExplorer->SetCurrentFile(filename);
    }
    
    return Load(filename);
}

void CodeEditor::Update(InputManager& input) {
    // Decrement saved message timer (Phase 2.0.4)
    if (savedMessageTimer > 0) {
        savedMessageTimer--;
    }
    
    // Decrement reloaded message timer (Phase 2.0.5.1)
    if (reloadedMessageTimer > 0) {
        reloadedMessageTimer--;
    }
    
    // Check for external file changes (Phase 2.0.5.1)
    CheckForExternalChanges();
    
    // === Handle scrollbar mouse input FIRST (Phase 2.0.5.3) ===
    HandleScrollbarInput(input);
    
    // === Handle mouse click to position cursor (Phase 2.0.5.3) ===
    if (input.isMouseButtonPressed(1)) {  // Left click
        int mouseX = input.getMouseX();
        int mouseY = input.getMouseY();
        
        // Layout constants (must match Render())
        const int TITLE_H = 10;
        const int STATUS_H = 10;
        const int SCREEN_H = 256;
        const int SIDEBAR_W = 180;
        const int SIDEBAR_OFFSET = (fileExplorer && fileExplorer->IsVisible()) ? SIDEBAR_W : 0;
        const int LINE_NUM_W = 40;
        const int TEXT_X = SIDEBAR_OFFSET + LINE_NUM_W + 4;
        const int EDITOR_TOP = TITLE_H;
        const int EDITOR_BOTTOM = SCREEN_H - STATUS_H;
        const int CHAR_W = 8;
        const int LINE_HEIGHT = 11;
        
        // Check if click is in text area (not in scrollbar, line numbers, or sidebar)
        bool inTextArea = (mouseX >= TEXT_X && mouseX < 252 &&  // Before scrollbar
                          mouseY >= EDITOR_TOP && mouseY < EDITOR_BOTTOM);
        
        if (inTextArea) {
            // Convert mouse Y to line number
            int clickLine = scrollY + ((mouseY - EDITOR_TOP - 2) / LINE_HEIGHT);
            if (clickLine < 0) clickLine = 0;
            if (clickLine >= static_cast<int>(lines.size())) clickLine = lines.size() - 1;
            
            // Convert mouse X to column
            int clickCol = scrollX + ((mouseX - TEXT_X) / CHAR_W);
            if (clickCol < 0) clickCol = 0;
            if (clickCol > static_cast<int>(lines[clickLine].length())) {
                clickCol = lines[clickLine].length();
            }
            
            // Move cursor to clicked position
            cursorLine = clickLine;
            cursorCol = clickCol;
        }
    }
    
    // File Explorer sidebar handling (Phase 2.0.5.2)
    if (fileExplorer) {
        // Toggle sidebar with Ctrl+L
        if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_L)) {
            fileExplorer->Toggle();
        }
        
        // If sidebar is visible, handle its input
        if (fileExplorer->IsVisible()) {
            fileExplorer->HandleInput(input);
            
            // Check if user selected a file
            std::string selectedFile = fileExplorer->GetSelectedFile();
            if (!selectedFile.empty() && selectedFile != currentFilename) {
                // Switch to the selected file
                // First, save current file if modified
                if (modified) {
                    Save();
                }
                
                // Load the new file
                if (Load(selectedFile)) {
                    currentFilename = selectedFile;
                    fileExplorer->SetCurrentFile(selectedFile);
                    fileExplorer->ClearSelection();
                }
            }
            
            // IMPORTANT: Block all other input while sidebar is visible
            // This prevents cursor movement and text editing in the background
            return;
        }
    }
    
    // === Handle mouse wheel scrolling (Phase 2.0.5.3) ===
    int wheelDelta = input.getMouseWheelY();
    if (wheelDelta != 0) {
        // Scroll 3 lines per wheel notch
        scrollY -= wheelDelta * 3;
        
        // Clamp scrollY
        int maxScroll = static_cast<int>(lines.size()) - 20;  // Approximate visible lines
        if (maxScroll < 0) maxScroll = 0;
        if (scrollY < 0) scrollY = 0;
        if (scrollY > maxScroll) scrollY = maxScroll;
    }
    
    // Handle text input (characters typed)
    if (input.hasTextInput()) {
        const std::string& text = input.getTextInput();
        for (char c : text) {
            // Filter out control characters (< 32)
            if (c >= 32 && c < 127) {  // Printable ASCII only
                InsertChar(c);
            }
        }
    }
    
    // === Cursor movement with auto-repeat ===
    // Left arrow
    if (input.isKeyDown(SDL_SCANCODE_LEFT)) {
        leftKeyHoldFrames++;
        // Move on first press OR after delay+interval
        if (leftKeyHoldFrames == 1 || 
            (leftKeyHoldFrames > keyRepeatDelay && (leftKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            MoveCursorLeft();
        }
    } else {
        leftKeyHoldFrames = 0;
    }
    
    // Right arrow
    if (input.isKeyDown(SDL_SCANCODE_RIGHT)) {
        rightKeyHoldFrames++;
        if (rightKeyHoldFrames == 1 || 
            (rightKeyHoldFrames > keyRepeatDelay && (rightKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            MoveCursorRight();
        }
    } else {
        rightKeyHoldFrames = 0;
    }
    
    // Up arrow
    if (input.isKeyDown(SDL_SCANCODE_UP)) {
        upKeyHoldFrames++;
        if (upKeyHoldFrames == 1 || 
            (upKeyHoldFrames > keyRepeatDelay && (upKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            MoveCursorUp();
        }
    } else {
        upKeyHoldFrames = 0;
    }
    
    // Down arrow
    if (input.isKeyDown(SDL_SCANCODE_DOWN)) {
        downKeyHoldFrames++;
        if (downKeyHoldFrames == 1 || 
            (downKeyHoldFrames > keyRepeatDelay && (downKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            MoveCursorDown();
        }
    } else {
        downKeyHoldFrames = 0;
    }
    
    // Home/End
    if (input.isKeyPressed(SDL_SCANCODE_HOME)) {
        MoveCursorHome();
    }
    if (input.isKeyPressed(SDL_SCANCODE_END)) {
        MoveCursorEnd();
    }
    
    // Page Up/Down
    if (input.isKeyPressed(SDL_SCANCODE_PAGEUP)) {
        PageUp();
    }
    if (input.isKeyPressed(SDL_SCANCODE_PAGEDOWN)) {
        PageDown();
    }
    
    // Backspace
    if (input.isKeyPressed(SDL_SCANCODE_BACKSPACE)) {
        Backspace();
    }
    
    // Delete
    if (input.isKeyPressed(SDL_SCANCODE_DELETE)) {
        Delete();
    }
    
    // Enter
    if (input.isKeyPressed(SDL_SCANCODE_RETURN)) {
        NewLine();
    }
    
    // Tab - Insert 4 spaces (Phase 2.0.4)
    if (input.isKeyPressed(SDL_SCANCODE_TAB)) {
        if (input.isShiftDown()) {
            // Shift+Tab - TODO: Unindent
        } else {
            // Tab - Insert 4 spaces
            for (int i = 0; i < 4; i++) {
                InsertChar(' ');
            }
        }
    }
    
    // Ctrl+S - Save (Phase 2.0.4)
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_S)) {
        if (Save()) {
            savedMessageTimer = 120;  // Show "SAVED!" for 2 seconds (120 frames @ 60fps)
        }
    }
    
    // Ctrl+R - Run/Reload (Phase 2.0.4)
    // Note: This will be handled by Engine to reload the cartridge
    // For now, we just save the file
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_R)) {
        Save();
        // Engine will detect file change via hot reload
    }
}

void CodeEditor::Render(AestheticLayer& layer, UISystem& ui) {
    // === Theme colors ===
    const int THEME_BG = UISystem::COLOR_BACKGROUND;
    const int THEME_LINE_NUM_BG = UISystem::COLOR_DARK_GRAY;
    const int THEME_LINE_NUM_TEXT = UISystem::COLOR_LIGHT_GRAY;
    const int THEME_BAR = UISystem::COLOR_LIGHT_GRAY;
    const int THEME_BAR_TEXT = UISystem::COLOR_BACKGROUND;
    
    // Clear screen
    layer.Clear(THEME_BG);
    
    // === Font is 8x8 monospace ===
    const int CHAR_W = 8;
    const int CHAR_H = 8;
    
    // CRITICAL: Line spacing must be LARGER than font height to prevent overlap
    const int LINE_HEIGHT = 11;  // 8px font + 3px spacing (NO overlap)
    
    // === Screen layout ===
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    
    const int TITLE_H = 10;
    const int STATUS_H = 10;
    
    // File explorer sidebar (Phase 2.0.5.2)
    const int SIDEBAR_W = 180; 
    const int SIDEBAR_OFFSET = (fileExplorer && fileExplorer->IsVisible()) ? SIDEBAR_W : 0;
    
    // Line numbers: 4 digits + space = "9999 " = 5 chars * 8px = 40px
    const int LINE_NUM_W = 40;
    const int LINE_NUM_X = SIDEBAR_OFFSET + 2;
    
    const int TEXT_X = SIDEBAR_OFFSET + LINE_NUM_W + 4;  // Start after sidebar + line numbers + 4px gap
    
    const int EDITOR_TOP = TITLE_H;
    const int EDITOR_BOTTOM = SCREEN_H - STATUS_H;
    const int EDITOR_H = EDITOR_BOTTOM - EDITOR_TOP;
    
    // === Title bar ===
    layer.RectFill(0, 0, SCREEN_W, TITLE_H, THEME_BAR);
    layer.Print("CODE", 4, 1, THEME_BAR_TEXT);
    
    // === Line number column ===
    // Start after sidebar if it's visible
    layer.RectFill(SIDEBAR_OFFSET, EDITOR_TOP, LINE_NUM_W, EDITOR_H, THEME_LINE_NUM_BG);
    
    // === Render code lines ===
    int y = EDITOR_TOP + 2;  // Start with small top margin
    
    for (int i = scrollY; i < static_cast<int>(lines.size()); i++) {
        // Check if we have room for this line
        if (y + CHAR_H > EDITOR_BOTTOM - 1) {
            break;
        }
        
        // Line number (4 digits, right-aligned)
        char lineNum[8];
        sprintf(lineNum, "%4d", i + 1);
        layer.Print(lineNum, LINE_NUM_X, y, THEME_LINE_NUM_TEXT);
        
        // Code text (with horizontal scroll)
        std::string lineText = lines[i];
        
        // Apply horizontal scroll: show only visible portion
        if (scrollX < static_cast<int>(lineText.length())) {
            lineText = lineText.substr(scrollX);
            if (lineText.length() > VISIBLE_COLS) {
                lineText = lineText.substr(0, VISIBLE_COLS);
            }
        } else {
            lineText = "";  // scrollX is beyond line length
        }
        
        RenderLineWithSyntax(lineText, TEXT_X, y, layer);
        
        y += LINE_HEIGHT;
    }
    
    // === Cursor ===
    if (cursorLine >= scrollY) {
        // Cursor X position accounts for horizontal scroll
        int cursorX = TEXT_X + ((cursorCol - scrollX) * CHAR_W);
        int cursorY = EDITOR_TOP + 2 + ((cursorLine - scrollY) * LINE_HEIGHT);
        
        if (cursorY + CHAR_H <= EDITOR_BOTTOM - 1) {
            static int blink = 0;
            blink = (blink + 1) % 60;
            if (blink < 30) {
                layer.RectFill(cursorX, cursorY, 2, CHAR_H, UISystem::COLOR_YELLOW);
            }
        }
    }
    
    // === Status bar ===
    int statusY = EDITOR_BOTTOM;
    layer.RectFill(0, statusY, SCREEN_W, STATUS_H, THEME_BAR);
    
    char status[32];
    if (savedMessageTimer > 0) {
        sprintf(status, "Ln%d SAVED", cursorLine + 1);
        layer.Print(status, 2, statusY + 1, UISystem::COLOR_GREEN);
    } else if (reloadedMessageTimer > 0) {
        sprintf(status, "Ln%d RELOAD", cursorLine + 1);
        layer.Print(status, 2, statusY + 1, UISystem::COLOR_YELLOW);
    } else {
        sprintf(status, "Ln%d:%d%s", cursorLine + 1, cursorCol + 1, 
                modified ? "*" : "");
        layer.Print(status, 2, statusY + 1, THEME_BAR_TEXT);
    }
    
    // Filename
    if (!currentFilename.empty()) {
        size_t lastSlash = currentFilename.find_last_of("/\\");
        std::string displayName = (lastSlash != std::string::npos) 
            ? currentFilename.substr(lastSlash + 1) 
            : currentFilename;
        
        int nameW = displayName.length() * CHAR_W;
        if (nameW < SCREEN_W - 80) {
            layer.Print(displayName, SCREEN_W - nameW - 2, statusY + 1, THEME_BAR_TEXT);
        }
    }
    
    // === Render scrollbar (Phase 2.0.5.3) ===
    // Scrollbar should be at the right edge of the screen
    // The area starts from SIDEBAR_OFFSET and goes to SCREEN_W
    int scrollbarAreaX = SIDEBAR_OFFSET;
    int scrollbarAreaW = SCREEN_W - SIDEBAR_OFFSET;
    RenderScrollbar(layer, scrollbarAreaX, EDITOR_TOP, scrollbarAreaW, EDITOR_H);
    
    // === Render file explorer sidebar LAST (Phase 2.0.5.2) ===
    // Draw it at the end so it appears on top of everything
    if (fileExplorer && fileExplorer->IsVisible()) {
        fileExplorer->Render(layer, ui, 0, EDITOR_TOP, SIDEBAR_W, EDITOR_H);
    }
}

bool CodeEditor::Save() {
    if (currentFilename.empty()) {
        return false;
    }
    
    std::ofstream file(currentFilename);
    if (!file.is_open()) {
        return false;
    }
    
    for (size_t i = 0; i < lines.size(); i++) {
        file << lines[i];
        if (i < lines.size() - 1) {
            file << "\n";
        }
    }
    
    file.close();
    modified = false;
    
    // Update file timestamp after save (Phase 2.0.5.1)
    if (std::filesystem::exists(currentFilename)) {
        lastFileWriteTime = std::filesystem::last_write_time(currentFilename);
    }
    
    return true;
}

bool CodeEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // File doesn't exist - start with empty buffer
        lines.clear();
        lines.push_back("");
        cursorLine = 0;
        cursorCol = 0;
        scrollY = 0;
        scrollX = 0;
        modified = false;
        return false;
    }
    
    lines.clear();
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    // Ensure at least one empty line
    if (lines.empty()) {
        lines.push_back("");
    }
    
    file.close();
    cursorLine = 0;
    cursorCol = 0;
    scrollY = 0;
    scrollX = 0;
    modified = false;
    
    // Initialize file watching (Phase 2.0.5.1)
    if (std::filesystem::exists(currentFilename)) {
        lastFileWriteTime = std::filesystem::last_write_time(currentFilename);
    }
    
    return true;
}

// ============================================
// TEXT BUFFER
// ============================================

std::string CodeEditor::GetText() const {
    std::string result;
    for (size_t i = 0; i < lines.size(); i++) {
        result += lines[i];
        if (i < lines.size() - 1) {
            result += "\n";
        }
    }
    return result;
}

void CodeEditor::SetText(const std::string& text) {
    lines.clear();
    
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    if (lines.empty()) {
        lines.push_back("");
    }
    
    cursorLine = 0;
    cursorCol = 0;
    scrollY = 0;
    scrollX = 0;
    modified = true;
}

void CodeEditor::InsertChar(char c) {
    if (cursorLine >= static_cast<int>(lines.size())) {
        return;
    }
    
    std::string& line = lines[cursorLine];
    line.insert(cursorCol, 1, c);
    cursorCol++;
    EnsureCursorVisibleHorizontal();
    modified = true;
}

void CodeEditor::Backspace() {
    if (cursorCol > 0) {
        // Delete character before cursor
        std::string& line = lines[cursorLine];
        line.erase(cursorCol - 1, 1);
        cursorCol--;
        modified = true;
    } else if (cursorLine > 0) {
        // Merge with previous line
        std::string currentLine = lines[cursorLine];
        lines.erase(lines.begin() + cursorLine);
        cursorLine--;
        cursorCol = lines[cursorLine].length();
        lines[cursorLine] += currentLine;
        modified = true;
        EnsureCursorVisible();
    }
}

void CodeEditor::Delete() {
    if (cursorCol < static_cast<int>(lines[cursorLine].length())) {
        // Delete character at cursor
        std::string& line = lines[cursorLine];
        line.erase(cursorCol, 1);
        modified = true;
    } else if (cursorLine < static_cast<int>(lines.size()) - 1) {
        // Merge with next line
        lines[cursorLine] += lines[cursorLine + 1];
        lines.erase(lines.begin() + cursorLine + 1);
        modified = true;
    }
}

void CodeEditor::NewLine() {
    std::string& currentLine = lines[cursorLine];
    std::string restOfLine = currentLine.substr(cursorCol);
    currentLine = currentLine.substr(0, cursorCol);
    
    cursorLine++;
    cursorCol = 0;
    lines.insert(lines.begin() + cursorLine, restOfLine);
    modified = true;
    EnsureCursorVisible();
}

// ============================================
// CURSOR MOVEMENT
// ============================================

void CodeEditor::MoveCursorLeft() {
    if (cursorCol > 0) {
        cursorCol--;
        EnsureCursorVisibleHorizontal();
    } else if (cursorLine > 0) {
        cursorLine--;
        cursorCol = lines[cursorLine].length();
        EnsureCursorVisible();
        EnsureCursorVisibleHorizontal();
    }
}

void CodeEditor::MoveCursorRight() {
    if (cursorCol < static_cast<int>(lines[cursorLine].length())) {
        cursorCol++;
        EnsureCursorVisibleHorizontal();
    } else if (cursorLine < static_cast<int>(lines.size()) - 1) {
        cursorLine++;
        cursorCol = 0;
        EnsureCursorVisible();
        EnsureCursorVisibleHorizontal();
    }
}

void CodeEditor::MoveCursorUp() {
    if (cursorLine > 0) {
        cursorLine--;
        ClampCursor();
        EnsureCursorVisible();
    }
}

void CodeEditor::MoveCursorDown() {
    if (cursorLine < static_cast<int>(lines.size()) - 1) {
        cursorLine++;
        ClampCursor();
        EnsureCursorVisible();
    }
}

void CodeEditor::MoveCursorHome() {
    cursorCol = 0;
    EnsureCursorVisibleHorizontal();
}

void CodeEditor::MoveCursorEnd() {
    cursorCol = lines[cursorLine].length();
    EnsureCursorVisibleHorizontal();
}

void CodeEditor::MoveCursorTop() {
    cursorLine = 0;
    cursorCol = 0;
    scrollY = 0;
    scrollX = 0;
}

void CodeEditor::MoveCursorBottom() {
    cursorLine = lines.size() - 1;
    cursorCol = lines[cursorLine].length();
    EnsureCursorVisible();
    EnsureCursorVisibleHorizontal();
}

void CodeEditor::PageUp() {
    cursorLine = std::max(0, cursorLine - VISIBLE_LINES);
    scrollY = std::max(0, scrollY - VISIBLE_LINES);
    ClampCursor();
}

void CodeEditor::PageDown() {
    cursorLine = std::min(static_cast<int>(lines.size()) - 1, 
                          cursorLine + VISIBLE_LINES);
    scrollY = std::min(static_cast<int>(lines.size()) - VISIBLE_LINES,
                       scrollY + VISIBLE_LINES);
    if (scrollY < 0) scrollY = 0;
    ClampCursor();
}

// ============================================
// PRIVATE HELPERS
// ============================================

void CodeEditor::EnsureCursorVisible() {
    // Scroll up if cursor is above visible area
    if (cursorLine < scrollY) {
        scrollY = cursorLine;
    }
    
    // Scroll down if cursor is below visible area
    if (cursorLine >= scrollY + VISIBLE_LINES) {
        scrollY = cursorLine - VISIBLE_LINES + 1;
    }
    
    // Clamp scroll
    if (scrollY < 0) {
        scrollY = 0;
    }
}

void CodeEditor::EnsureCursorVisibleHorizontal() {
    // Scroll left if cursor is before visible area
    if (cursorCol < scrollX) {
        scrollX = cursorCol;
    }
    
    // Scroll right if cursor is after visible area
    if (cursorCol >= scrollX + VISIBLE_COLS) {
        scrollX = cursorCol - VISIBLE_COLS + 1;
    }
    
    // Clamp scroll (can't be negative)
    if (scrollX < 0) {
        scrollX = 0;
    }
}

void CodeEditor::ClampCursor() {
    // Clamp cursor column to current line length
    if (cursorCol > static_cast<int>(lines[cursorLine].length())) {
        cursorCol = lines[cursorLine].length();
    }
}

// ============================================
// EXTERNAL FILE CHANGE DETECTION (Phase 2.0.5.1)
// ============================================

void CodeEditor::CheckForExternalChanges() {
    // Only check if we have a file and watching is enabled
    if (currentFilename.empty() || !fileWatchingEnabled) {
        return;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(currentFilename)) {
        return;
    }
    
    try {
        // Get current file modification time
        auto currentFileTime = std::filesystem::last_write_time(currentFilename);
        
        // Check if file has been modified externally
        if (currentFileTime != lastFileWriteTime) {
            // Save cursor position
            int savedCursorLine = cursorLine;
            int savedCursorCol = cursorCol;
            int savedScrollY = scrollY;
            int savedScrollX = scrollX;
            
            // Reload file
            if (Load(currentFilename)) {
                // Try to restore cursor position (if still valid)
                if (savedCursorLine < static_cast<int>(lines.size())) {
                    cursorLine = savedCursorLine;
                    if (savedCursorCol <= static_cast<int>(lines[cursorLine].length())) {
                        cursorCol = savedCursorCol;
                    } else {
                        cursorCol = lines[cursorLine].length();
                    }
                    scrollY = savedScrollY;
                    scrollX = savedScrollX;
                    EnsureCursorVisible();
                    EnsureCursorVisibleHorizontal();
                }
                
                // Update timestamp
                lastFileWriteTime = currentFileTime;
                
                // Show notification
                reloadedMessageTimer = 120;  // Show "RELOADED!" for 2 seconds
                
                // Reset modified flag since we just loaded from disk
                modified = false;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Ignore filesystem errors (file might be temporarily locked)
    }
}

// ============================================
// SCROLLBAR INPUT HANDLING (Phase 2.0.5.3)
// ============================================

void CodeEditor::HandleScrollbarInput(InputManager& input) {
    // Scrollbar dimensions (must match RenderScrollbar)
    const int SCROLLBAR_W = 4;  // Simplified to 4px
    const int MIN_THUMB_H = 20;
    const int LINE_HEIGHT = 11;
    
    // Editor layout (must match Render())
    const int TITLE_H = 10;
    const int STATUS_H = 10;
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    const int SIDEBAR_W = 180;
    const int SIDEBAR_OFFSET = (fileExplorer && fileExplorer->IsVisible()) ? SIDEBAR_W : 0;
    const int EDITOR_TOP = TITLE_H;
    const int EDITOR_BOTTOM = SCREEN_H - STATUS_H;
    const int EDITOR_H = EDITOR_BOTTOM - EDITOR_TOP;
    
    int totalLines = static_cast<int>(lines.size());
    int visibleLines = EDITOR_H / LINE_HEIGHT;
    
    // Don't handle scrollbar if all lines fit
    if (totalLines <= visibleLines) {
        scrollbarDragging = false;
        return;
    }
    
    // Calculate scrollbar area (must match RenderScrollbar)
    int scrollbarAreaW = SCREEN_W - SIDEBAR_OFFSET;
    int scrollbarX = SIDEBAR_OFFSET + scrollbarAreaW - SCROLLBAR_W;
    int scrollbarY = EDITOR_TOP;
    int scrollbarH = EDITOR_H;
    
    // Get mouse position and button state
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    bool mouseDown = input.isMouseButtonDown(1);  // SDL uses 1 for left button!
    bool mousePressed = input.isMouseButtonPressed(1);
    
    // Check if mouse is over scrollbar area
    bool overScrollbar = (mouseX >= scrollbarX && mouseX < scrollbarX + SCROLLBAR_W &&
                          mouseY >= scrollbarY && mouseY < scrollbarY + scrollbarH);
    
    if (!overScrollbar && !scrollbarDragging) {
        return;
    }
    
    // Calculate thumb position  
    int trackY = scrollbarY;
    int trackH = scrollbarH;
    
    float visibleRatio = static_cast<float>(visibleLines) / static_cast<float>(totalLines);
    int thumbH = static_cast<int>(trackH * visibleRatio);
    if (thumbH < MIN_THUMB_H) thumbH = MIN_THUMB_H;
    if (thumbH > trackH) thumbH = trackH;
    
    int maxScroll = totalLines - visibleLines;
    if (maxScroll <= 0) maxScroll = 1;
    
    float scrollRatio = static_cast<float>(scrollY) / static_cast<float>(maxScroll);
    int thumbTravelDistance = trackH - thumbH;
    int thumbY = trackY + static_cast<int>(thumbTravelDistance * scrollRatio);
    
    // === Handle thumb dragging ===
    if (scrollbarDragging) {
        if (mouseDown) {
            // Calculate new scroll position from mouse Y
            int dragY = mouseY - scrollbarDragOffset;
            int relativeY = dragY - trackY;
            
            if (thumbTravelDistance > 0) {
                float newScrollRatio = static_cast<float>(relativeY) / static_cast<float>(thumbTravelDistance);
                scrollY = static_cast<int>(newScrollRatio * maxScroll);
                
                // Clamp
                if (scrollY < 0) scrollY = 0;
                if (scrollY > maxScroll) scrollY = maxScroll;
            }
        } else {
            scrollbarDragging = false;
        }
        return;
    }
    
    // === Start dragging on click ===
    if (mousePressed && overScrollbar) {
        // Check if clicked on thumb
        if (mouseY >= thumbY && mouseY < thumbY + thumbH) {
            scrollbarDragging = true;
            scrollbarDragOffset = mouseY - thumbY;
        }
    }
}

// ============================================
// SCROLLBAR RENDERING (Phase 2.0.5.3)
// ============================================

void CodeEditor::RenderScrollbar(AestheticLayer& layer, int x, int y, int width, int height) {
    const int SCROLLBAR_W = 4;  // Simplified to 4px
    const int MIN_THUMB_H = 20;
    
    // Don't show scrollbar if all lines fit on screen
    int totalLines = static_cast<int>(lines.size());
    int visibleLines = height / 11;  // LINE_HEIGHT from Render()
    
    if (totalLines <= visibleLines) {
        return;  // No need for scrollbar
    }
    
    // Colors
    const int COLOR_TRACK = UISystem::COLOR_DARK_GRAY;
    const int COLOR_THUMB = UISystem::COLOR_WHITE;  // White for visibility
    
    // Scrollbar position (right edge of area)
    int scrollbarX = x + width - SCROLLBAR_W;
    int scrollbarY = y;
    int scrollbarH = height;
    
    // === Draw track (background) ===
    layer.RectFill(scrollbarX, scrollbarY, SCROLLBAR_W, scrollbarH, COLOR_TRACK);
    
    // === Calculate thumb position and size ===
    int trackY = scrollbarY;
    int trackH = scrollbarH;
    
    // Thumb size proportional to visible/total ratio
    float visibleRatio = static_cast<float>(visibleLines) / static_cast<float>(totalLines);
    int thumbH = static_cast<int>(trackH * visibleRatio);
    if (thumbH < MIN_THUMB_H) {
        thumbH = MIN_THUMB_H;
    }
    if (thumbH > trackH) {
        thumbH = trackH;
    }
    
    // Thumb position based on scrollY
    int maxScroll = totalLines - visibleLines;
    if (maxScroll <= 0) maxScroll = 1;
    
    float scrollRatio = static_cast<float>(scrollY) / static_cast<float>(maxScroll);
    int thumbTravelDistance = trackH - thumbH;
    int thumbY = trackY + static_cast<int>(thumbTravelDistance * scrollRatio);
    
    // === Draw thumb ===
    layer.RectFill(scrollbarX, thumbY, SCROLLBAR_W, thumbH, COLOR_THUMB);
}

// ============================================
// SYNTAX HIGHLIGHTING (Phase 2.0.3)
// ============================================

void CodeEditor::RenderLineWithSyntax(const std::string& line, int x, int y, AestheticLayer& layer) {
    const int CHAR_W = 8;  // CRITICAL: Font is 8 pixels wide
    int currentX = x;
    size_t i = 0;
    
    while (i < line.length()) {
        char c = line[i];
        
        // Whitespace
        if (LuaSyntax::IsWhitespace(c)) {
            currentX += CHAR_W;
            i++;
            continue;
        }
        
        // Comments (-- to end of line)
        if (c == '-' && i + 1 < line.length() && line[i + 1] == '-') {
            std::string comment = line.substr(i);
            layer.Print(comment, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::COMMENT));
            break;
        }
        
        // Strings
        if (c == '"' || c == '\'') {
            char quote = c;
            size_t endQuote = i + 1;
            
            while (endQuote < line.length() && line[endQuote] != quote) {
                endQuote++;
            }
            
            if (endQuote < line.length()) {
                endQuote++;
            }
            
            std::string str = line.substr(i, endQuote - i);
            layer.Print(str, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::STRING));
            currentX += str.length() * CHAR_W;
            i = endQuote;
            continue;
        }
        
        // Numbers
        if (LuaSyntax::IsDigit(c) || (c == '-' && i + 1 < line.length() && LuaSyntax::IsDigit(line[i + 1]))) {
            size_t numEnd = i;
            if (c == '-') numEnd++;
            
            while (numEnd < line.length() && 
                   (LuaSyntax::IsDigit(line[numEnd]) || line[numEnd] == '.')) {
                numEnd++;
            }
            
            std::string num = line.substr(i, numEnd - i);
            layer.Print(num, currentX, y, LuaSyntax::GetColorForToken(LuaSyntax::TokenType::NUMBER));
            currentX += num.length() * CHAR_W;
            i = numEnd;
            continue;
        }
        
        // Identifiers/Keywords
        if (LuaSyntax::IsIdentifierStart(c)) {
            size_t identEnd = i;
            while (identEnd < line.length() && LuaSyntax::IsIdentifierChar(line[identEnd])) {
                identEnd++;
            }
            
            std::string ident = line.substr(i, identEnd - i);
            LuaSyntax::TokenType tokenType = LuaSyntax:: GetTokenType(ident);
            
            layer.Print(ident, currentX, y, LuaSyntax::GetColorForToken(tokenType));
            currentX += ident.length() * CHAR_W;
            i = identEnd;
            continue;
        }
        
        // Single character (operators, punctuation)
        std::string ch(1, c);
        layer.Print(ch, currentX, y, UISystem::COLOR_PEACH);
        currentX += CHAR_W;
        i++;
    }
}
