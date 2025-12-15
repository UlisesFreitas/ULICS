#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <memory>

// Forward declarations
class AestheticLayer;
class InputManager;
class UISystem;
class FileExplorer;

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
    int scrollX;     // First visible column (horizontal scroll)

    // File info
    std::string currentFilename;
    bool modified;   // Has file been modified since last save?
    int savedMessageTimer;  // Frames to show "SAVED!" message (Phase 2.0.4)
    int reloadedMessageTimer;  // Frames to show "RELOADED!" message (Phase 2.0.5.1)
    
    // External file change detection (Phase 2.0.5.1)
    std::filesystem::file_time_type lastFileWriteTime;
    bool fileWatchingEnabled;
    
    // File explorer sidebar (Phase 2.0.5.2)
    std::unique_ptr<FileExplorer> fileExplorer;
    std::string cartridgePath;  // Current cartridge directory
    
    // Scrollbar state (Phase 2.0.5.3)
    bool scrollbarDragging;      // Is user dragging the thumb?
    int scrollbarDragOffset;     // Offset from thumb top when drag started
    
    // Text selection (Phase 2.0.5.4)
    bool selectionActive;
    int selectionStartLine;
    int selectionStartCol;
    int selectionEndLine;
    int selectionEndCol;
    bool mouseDragging;  // Is user dragging to select text?
    
    // Undo/Redo system (Phase 2.0.5.4)
    struct EditAction {
        enum Type { INSERT_CHAR, DELETE_CHAR, INSERT_LINE, DELETE_LINE, REPLACE_TEXT };
        Type type;
        int line;
        int col;
        std::string oldText;
        std::string newText;
    };
    std::vector<EditAction> undoStack;
    std::vector<EditAction> redoStack;
    const int MAX_UNDO_STACK = 100;

    // Key repeat for cursor movement (auto-repeat when holding arrow keys)
    int keyRepeatDelay;      // Frames before repeat starts
    int keyRepeatInterval;   // Frames between repeats
    int leftKeyHoldFrames;
    int rightKeyHoldFrames;
    int upKeyHoldFrames;
    int downKeyHoldFrames;

    // Editor dimensions
    // Font: 8x8px monospace (EmbeddedFont.h)
    // Line height: 11px (8px + 3px spacing - prevents overlap)
    // Line numbers: 40px (4 digits support up to 9999 lines)
    // Text area: 256 - 40 - 4 = 212px (212/8 = 26 chars)
    // Editor height: 256 - 10 - 10 = 236px (236/11 = 21 lines)
    static constexpr int VISIBLE_LINES = 22;
    static constexpr int VISIBLE_COLS = 26;

    // Helper functions
    void EnsureCursorVisible();              // Vertical scroll
    void EnsureCursorVisibleHorizontal();    // Horizontal scroll
    void ClampCursor();
    void CheckForExternalChanges();          // File watching (Phase 2.0.5.1)
    void HandleScrollbarInput(InputManager& input);  // Scrollbar mouse handling (Phase 2.0.5.3)
    void RenderScrollbar(AestheticLayer& layer, int x, int y, int width, int height);  // Scrollbar (Phase 2.0.5.3)
    
    // Selection and clipboard helpers (Phase 2.0.5.4)
    void ClearSelection();
    bool HasSelection() const;
    std::string GetSelectedText() const;
    void DeleteSelection();
    void NormalizeSelection();  // Ensure start comes before end
    
    // Undo/Redo helpers (Phase 2.0.5.4)
    void PushUndo(EditAction action);
    void Undo();
    void Redo();
    void ClearRedoStack();
    
    // Syntax highlighting (Phase 2.0.3)
    void RenderLineWithSyntax(const std::string& line, int x, int y, AestheticLayer& layer);
};

#endif // CODEEDITOR_H
