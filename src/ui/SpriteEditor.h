#ifndef SPRITE_EDITOR_H
#define SPRITE_EDITOR_H

#include <string>
#include <fstream>
#include <cstdint>
#include <vector>

// Forward declarations
class AestheticLayer;
class InputManager;
class SystemSprites;

/**
 * @brief Sprite Editor for ULICS Fantasy Console
 * 
 * PICO-8/TIC-80 style pixel art editor with custom retro UI.
 * Features:
 * - 8x8 pixel sprites with 8x zoom (64x64 display)
 * - 16-color palette picker
 * - Drawing tools: Pencil, Fill, Line, Rectangle, Color Picker
 * - 256 sprite slots (16x16 grid in spritesheet)
 * - Save/Load to PNG spritesheet
 * 
 * Phase 3 - Custom UI using AestheticLayer primitives only.
 */
class SpriteEditor {
public:
    SpriteEditor();
    ~SpriteEditor();
    
    // Core methods
    void Update(InputManager& input);
    void Render(AestheticLayer& renderer, InputManager& input);
    
    // Initialization and persistence
    void Initialize(const std::string& spritesheetPath);
    void SaveSpritesheet();
    void LoadSpritesheet();
    
    // State queries
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }
    
    // Set system sprites for icons
    void SetSystemSprites(SystemSprites* sprites) { systemSprites = sprites; }
    
    // Undo/Redo
    void Undo();
    void Redo();
    
    // Import/Export
    void OnImportClicked();
    void OnExportClicked();
    void OnExportCurrentSprite();  // Export only current sprite
    void OnFileDropped(const char* filepath);  // Drag & drop handler
    
private:
    // Tool types
    enum class Tool {
        PENCIL,   // Draw single pixels
        FILL,     // Flood fill
        LINE,     // Draw lines
        RECT,     // Draw rectangles
        PICKER    // Color picker (eyedropper)
    };
    
    // Canvas state
    uint8_t canvas[8][8];           // Current sprite being edited (8x8 pixels)
    uint8_t spriteSheet[256][8][8]; // All 256 sprites
    int currentSpriteIndex;          // 0-255
    
    // Tool and color state
    Tool currentTool;
    int selectedColor;  // 0-15 (palette index)
    
    // UI state
    bool isActive;
    int zoom;  // Pixels per sprite pixel (16 = 128x128 display)
    bool showGrid;  // Toggle grid visibility (hotkey: G)
    bool filledRectMode;  // Toggle filled/outline rectangle (hotkey: X)
    
    // Drag state for line/rect tools
    bool isDragging;
    int dragStartX;
    int dragStartY;
    
    // Undo/Redo system (simple stack approach)
    struct CanvasState {
        uint8_t data[8][8];
    };
    static constexpr int MAX_UNDO_LEVELS = 50;
    std::vector<CanvasState> undoStack;  // States before each modification
    std::vector<CanvasState> redoStack;  // States that were undone
    
    // Clipboard for copy/paste (Ctrl+C / Ctrl+V)
    uint8_t clipboard[8][8];
    bool hasClipboardData;
    
    // Recent files tracking
    static constexpr int MAX_RECENT_FILES = 5;
    std::vector<std::string> recentFiles;
    
    // UI layout constants - PICO-8 style (Vertical layout)
    // Title and status bars (matching CodeEditor style)
    static constexpr int TITLE_BAR_H = 10;
    static constexpr int STATUS_BAR_H = 10;
    
    // Main canvas (top left, 8x8 sprite with 16x zoom)
    static constexpr int CANVAS_X = 16;
    static constexpr int CANVAS_Y = 18;  // Was 8, +10 for title bar
    static constexpr int CANVAS_ZOOM = 16;  // 16x zoom = 128x128 display
    static constexpr int CANVAS_SIZE = 8 * CANVAS_ZOOM;  // 128px
    
    // Utility bar (vertical, right of canvas)
    static constexpr int UTILITY_BAR_X = CANVAS_X + CANVAS_SIZE + 4;  // 148
    static constexpr int UTILITY_BAR_Y = CANVAS_Y;
    static constexpr int UTILITY_BUTTON_SIZE = 16;
    static constexpr int UTILITY_BUTTON_SPACING = 4;
    
    // Palette (right side, 4x4 square grid) - moved right
    static constexpr int PALETTE_X = 176;  // Was 152, moved +24 for utility bar
    static constexpr int PALETTE_Y = 30;  // Was 20, +10 for title bar
    static constexpr int PALETTE_COLS = 4;
    static constexpr int PALETTE_ROWS = 4;
    static constexpr int COLOR_BOX_SIZE = 12;
    
    // Toolbar (below canvas, left side) - 4px gap from spritesheet
    static constexpr int TOOLBAR_X = 16;
    static constexpr int TOOLBAR_Y = CANVAS_Y + CANVAS_SIZE + 4;  // 150 (18+128+4) - was +5
    
    // Spritesheet grid (below toolbar, 16x8 = 128 visible sprites)
    static constexpr int SHEET_X = 8;
    static constexpr int SHEET_Y = TOOLBAR_Y + 32;  // 186 (toolbar 16px + spacing)
    static constexpr int SHEET_COLS = 16;
    static constexpr int SHEET_ROWS = 8;
    static constexpr int SHEET_SPRITE_SIZE = 8;  // Each sprite 8x8px
    
    // Rendering methods
    void RenderCanvas(AestheticLayer& renderer);
    void RenderPalette(AestheticLayer& renderer);
    void RenderSpritesheet(AestheticLayer& renderer);
    void RenderToolbar(AestheticLayer& renderer);
    void RenderUtilityBar(AestheticLayer& renderer);  // NEW: vertical icon bar
    void RenderHeader(AestheticLayer& renderer);
    void RenderCursorHighlight(AestheticLayer& renderer, InputManager& input);
    void RenderDragPreview(AestheticLayer& renderer, InputManager& input);
    
    // Input handling
    void HandleCanvasClick(int mouseX, int mouseY);
    void HandlePaletteClick(int mouseX, int mouseY);
    void HandleToolbarClick(int mouseX, int mouseY);
    void HandleUtilityButtonClick(int index);  // Helper for utility bar clicks
    void HandleKeyboard(InputManager& input);
    
    // Drawing tools implementation
    void UsePencil(int x, int y);
    void UseFill(int x, int y, uint8_t targetColor, uint8_t replacementColor);
    void UseLine(int x1, int y1, int x2, int y2);
    void UseRect(int x1, int y1, int x2, int y2, bool filled);
    void UsePicker(int x, int y);
    
    // Transform tools
    void MirrorHorizontal();  // Flip sprite left-right
    void MirrorVertical();    // Flip sprite top-bottom
    void RotateLeft();   // Rotate 90° counter-clockwise
    void RotateRight();  // Rotate 90° clockwise
    
    // Helper methods
    void ClearCanvas();
    void SwitchSprite(int newIndex);
    int ScreenToCanvasX(int screenX);
    int ScreenToCanvasY(int screenY);
    bool IsInCanvas(int canvasX, int canvasY);
    
    // Undo/Redo helpers
    void SaveHistoryState();  // Save current canvas before modification
    
    // Import/Export helpers
    bool ImportSpritesheet(const std::string& filepath);
    bool ExportSpritesheet(const std::string& filepath);
    bool ExportSingleSprite(int spriteIndex, const std::string& filepath, int scale = 1);
    bool ValidatePNG(const std::string& filepath, int& width, int& height);
    
    // Recent files management
    void AddToRecentFiles(const std::string& filepath);
    void LoadRecentFiles();
    void SaveRecentFiles();
    
    // Debug logging
    void Log(const std::string& message);
    std::ofstream logFile;
    
    // System sprites for UI icons
    SystemSprites* systemSprites;
    
    // Spritesheet path
    std::string spritesheetPath;
};

#endif // SPRITE_EDITOR_H
