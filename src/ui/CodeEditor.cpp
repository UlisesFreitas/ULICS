#include "ui/CodeEditor.h"
#include "ui/UISystem.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "input/InputConstants.h"
#include <fstream>
#include <sstream>
#include <algorithm>

CodeEditor::CodeEditor()
    : cursorLine(0), cursorCol(0), scrollY(0), modified(false) {
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
    return Load(filename);
}

void CodeEditor::Update(InputManager& input) {
    // Handle text input
    // TODO Phase 2.0.3: We'll add SDL_TEXTINPUT event handling
    
    // Handle cursor movement
    if (input.isKeyPressed(SDL_SCANCODE_LEFT)) {
        MoveCursorLeft();
    }
    if (input.isKeyPressed(SDL_SCANCODE_RIGHT)) {
        MoveCursorRight();
    }
    if (input.isKeyPressed(SDL_SCANCODE_UP)) {
        MoveCursorUp();
    }
    if (input.isKeyPressed(SDL_SCANCODE_DOWN)) {
        MoveCursorDown();
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
    
    // TODO Phase 2.0.4: Ctrl+S (Save), Ctrl+R (Run), Ctrl+C/V (Copy/Paste)
}

void CodeEditor::Render(AestheticLayer& layer, UISystem& ui) {
    // Clear screen
    layer.Clear(UISystem::COLOR_BACKGROUND);
    
    // Draw title bar
    ui.RenderPanel(layer, 0, 0, 256, 256, "CODE EDITOR", true);
    
    // Editor area starts at y=12 (after title bar)
    int editorX = 4;
    int editorY = 14;
    int lineNumberWidth = 20;  // 3 digits  + space
    int textX = editorX + lineNumberWidth;
    
    // Draw line numbers background
    layer.RectFill(editorX, editorY, lineNumberWidth - 2, 220, UISystem::COLOR_DARK_GRAY);
    
    // Render visible lines
    int lineHeight = 8;  // 6px font + 2px spacing
    int y = editorY;
    
    for (int i = scrollY; i < scrollY + VISIBLE_LINES && i < static_cast<int>(lines.size()); i++) {
        // Line number
        char lineNum[5];
        sprintf(lineNum, "%3d", i + 1);
        layer.Print(lineNum, editorX + 2, y, UISystem::COLOR_LIGHT_GRAY);
        
        // Line text (truncate if too long)
        std::string lineText = lines[i];
        if (lineText.length() > VISIBLE_COLS) {
            lineText = lineText.substr(0, VISIBLE_COLS);
        }
        
        layer.Print(lineText, textX, y, UISystem::COLOR_WHITE);
        
        y += lineHeight;
    }
    
    // Draw cursor (blinking box)
    if (cursorLine >= scrollY && cursorLine < scrollY + VISIBLE_LINES) {
        int cursorX = textX + (cursorCol * 4);
        int cursorY = editorY + ((cursorLine - scrollY) * lineHeight);
        
        // Simple blinking (TODO: use time-based blinking)
        static int blink = 0;
        blink = (blink + 1) % 60;
        if (blink < 30) {
            layer.RectFill(cursorX, cursorY, 2, 6, UISystem::COLOR_YELLOW);
        }
    }
    
    // Status bar at bottom
    int statusY = 244;
    layer.RectFill(0, statusY, 256, 12, UISystem::COLOR_DARK_PURPLE);
    
    // Status text
    char status[64];
    sprintf(status, "Line %d:%d  %s", cursorLine + 1, cursorCol + 1, 
            modified ? "MODIFIED" : "");
    layer.Print(status, 4, statusY + 3, UISystem::COLOR_WHITE);
    
    // File name (right-aligned)
    if (!currentFilename.empty()) {
        // Get just the filename (not full path)
        size_t lastSlash = currentFilename.find_last_of("/\\");
        std::string displayName = (lastSlash != std::string::npos) 
            ? currentFilename.substr(lastSlash + 1) 
            : currentFilename;
        
        int nameWidth = displayName.length() * 4;
        layer.Print(displayName, 256 - nameWidth - 4, statusY + 3, UISystem::COLOR_PEACH);
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
    modified = false;
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
    modified = true;
}

void CodeEditor::InsertChar(char c) {
    if (cursorLine >= static_cast<int>(lines.size())) {
        return;
    }
    
    std::string& line = lines[cursorLine];
    line.insert(cursorCol, 1, c);
    cursorCol++;
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
    } else if (cursorLine > 0) {
        cursorLine--;
        cursorCol = lines[cursorLine].length();
        EnsureCursorVisible();
    }
}

void CodeEditor::MoveCursorRight() {
    if (cursorCol < static_cast<int>(lines[cursorLine].length())) {
        cursorCol++;
    } else if (cursorLine < static_cast<int>(lines.size()) - 1) {
        cursorLine++;
        cursorCol = 0;
        EnsureCursorVisible();
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
}

void CodeEditor::MoveCursorEnd() {
    cursorCol = lines[cursorLine].length();
}

void CodeEditor::MoveCursorTop() {
    cursorLine = 0;
    cursorCol = 0;
    scrollY = 0;
}

void CodeEditor::MoveCursorBottom() {
    cursorLine = lines.size() - 1;
    cursorCol = lines[cursorLine].length();
    EnsureCursorVisible();
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

void CodeEditor::ClampCursor() {
    // Clamp cursor column to current line length
    if (cursorCol > static_cast<int>(lines[cursorLine].length())) {
        cursorCol = lines[cursorLine].length();
    }
}
