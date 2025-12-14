#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <string>
#include <vector>

// Forward declarations
class AestheticLayer;
class InputManager;
class UISystem;

/**
 * @brief CodeEditor - In-engine Lua code editor
 * 
 * THIS IS THE CORE FANTASY CONSOLE FEATURE!
 * Like PICO-8/TIC-80, ULICS launches into a code editor by default.
 * 
 * Features (Phase 2.0.2 - Basic):
 * - Multi-line text buffer
 * - Cursor movement (arrows, home/end, page up/down)
 * - Text insertion/deletion
 * - Line numbers
 * - Vertical scrolling
 * - Status bar (line:col, file info)
 * 
 * Future (Phase 2.0.3):
 * - Syntax highlighting
 * - Copy/paste
 * - Undo/redo
 * 
 * Controls:
 * - Arrow keys: Move cursor
 * - Home/End: Start/end of line
 * - Page Up/Down: Scroll page
 * - Ctrl+S: Save
 * - Ctrl+R: Run
 * - Escape: Toggle to game view
 */
class CodeEditor {
public:
    CodeEditor();
    ~CodeEditor();

    // ============================================
    // CORE FUNCTIONS
    // ============================================
    
    /**
     * @brief Initialize editor and load file
     * @param filename File to load (e.g., "cartridges/mygame/main.lua")
     */
    bool Initialize(const std::string& filename);

    /**
     * @brief Update editor (handle input)
     */
    void Update(InputManager& input);

    /**
     * @brief Render editor
     */
    void Render(AestheticLayer& layer, UISystem& ui);

    /**
     * @brief Save current buffer to file
     */
    bool Save();

    /**
     * @brief Load file into buffer
     */
    bool Load(const std::string& filename);

    // ============================================
    // TEXT BUFFER
    // ============================================
    
    /**
     * @brief Get current text content
     */
    std::string GetText() const;

    /**
     * @brief Set text content
     */
    void SetText(const std::string& text);

    /**
     * @brief Insert character at cursor
     */
    void InsertChar(char c);

    /**
     * @brief Delete character before cursor (backspace)
     */
    void Backspace();

    /**
     * @brief Delete character at cursor (delete)
     */
    void Delete();

    /**
     * @brief Insert newline at cursor
     */
    void NewLine();

    // ============================================
    // CURSOR MOVEMENT
    // ============================================
    
    /**
     * @brief Move cursor left
     */
    void MoveCursorLeft();

    /**
     * @brief Move cursor right
     */
    void MoveCursorRight();

    /**
     * @brief Move cursor up
     */
    void MoveCursorUp();

    /**
     * @brief Move cursor down
     */
    void MoveCursorDown();

    /**
     * @brief Move cursor to start of line
     */
    void MoveCursorHome();

    /**
     * @brief Move cursor to end of line
     */
    void MoveCursorEnd();

    /**
     * @brief Move cursor to start of file
     */
    void MoveCursorTop();

    /**
     * @brief Move cursor to end of file
     */
    void MoveCursorBottom();

    /**
     * @brief Page up (scroll up one screen)
     */
    void PageUp();

    /**
     * @brief Page down (scroll down one screen)
     */
    void PageDown();

private:
    // Text buffer (one string per line)
    std::vector<std::string> lines;

    // Cursor position (0-indexed)
    int cursorLine;  // Current line
    int cursorCol;   // Current column

    // Scroll position (0-indexed)
    int scrollY;     // First visible line

    // File info
    std::string currentFilename;
    bool modified;   // Has file been modified since last save?
    int savedMessageTimer;  // Frames to show "SAVED!" message (Phase 2.0.4)

    // Editor dimensions (in characters)
    static constexpr int VISIBLE_LINES = 28;  // ~224 pixels / 8 pixels per line
    static constexpr int VISIBLE_COLS = 58;   // ~232 pixels / 4 pixels per char (with line numbers)

    // Helper functions
    void EnsureCursorVisible();
    void ClampCursor();
    
    // Syntax highlighting (Phase 2.0.3)
    void RenderLineWithSyntax(const std::string& line, int x, int y, AestheticLayer& layer);
};

#endif // CODEEDITOR_H
