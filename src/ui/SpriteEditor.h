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
    void Render(AestheticLayer& renderer);
    
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
    
    // Drag state for line/rect tools
    bool isDragging;
    int dragStartX;
    int dragStartY;
    
    // Undo/Redo system
    struct CanvasState {
        uint8_t data[8][8];
    };
    static constexpr int MAX_HISTORY = 50;
    std::vector<CanvasState> undoHistory;
    int historyIndex;  // Current position in history (-1 = no history)
    
    // UI layout constants - PICO-8 style (Vertical layout)
    // Main canvas (top left, 8x8 sprite with 16x zoom)
    static constexpr int CANVAS_X = 16;
    static constexpr int CANVAS_Y = 8;
    static constexpr int CANVAS_ZOOM = 16;  // 16x zoom = 128x128 display
    static constexpr int CANVAS_SIZE = 8 * CANVAS_ZOOM;  // 128px
    
    // Palette (right side, 4x4 square grid)
    static constexpr int PALETTE_X = 152;
    static constexpr int PALETTE_Y = 20;
    static constexpr int PALETTE_COLS = 4;
    static constexpr int PALETTE_ROWS = 4;
    static constexpr int COLOR_BOX_SIZE = 12;
    
    // Toolbar (below canvas, left side)
    static constexpr int TOOLBAR_X = 16;
    static constexpr int TOOLBAR_Y = CANVAS_Y + CANVAS_SIZE + 8;  // 144
    
    // Spritesheet grid (below toolbar, 16x8 = 128 visible sprites)
    static constexpr int SHEET_X = 8;
    static constexpr int SHEET_Y = TOOLBAR_Y + 32;  // 176 (toolbar 16px + spacing)
    static constexpr int SHEET_COLS = 16;
    static constexpr int SHEET_ROWS = 8;
    static constexpr int SHEET_SPRITE_SIZE = 8;  // Each sprite 8x8px
    
    // Rendering methods
    void RenderCanvas(AestheticLayer& renderer);
    void RenderPalette(AestheticLayer& renderer);
    void RenderSpritesheet(AestheticLayer& renderer);
    void RenderToolbar(AestheticLayer& renderer);
    void RenderHeader(AestheticLayer& renderer);
    
    // Input handling
    void HandleCanvasClick(int mouseX, int mouseY);
    void HandlePaletteClick(int mouseX, int mouseY);
    void HandleToolbarClick(int mouseX, int mouseY);
    void HandleKeyboard(InputManager& input);
    
    // Drawing tools implementation
    void UsePencil(int x, int y);
    void UseFill(int x, int y, uint8_t targetColor, uint8_t replacementColor);
    void UseLine(int x1, int y1, int x2, int y2);
    void UseRect(int x1, int y1, int x2, int y2, bool filled);
    void UsePicker(int x, int y);
    
    // Helper methods
    void ClearCanvas();
    void SwitchSprite(int newIndex);
    int ScreenToCanvasX(int screenX);
    int ScreenToCanvasY(int screenY);
    bool IsInCanvas(int canvasX, int canvasY);
    
    // Undo/Redo helpers
    void SaveHistoryState();  // Save current canvas before modification
    
    // Debug logging
    void Log(const std::string& message);
    std::ofstream logFile;
    
    // System sprites for UI icons
    SystemSprites* systemSprites;
    
    // Spritesheet path
    std::string spritesheetPath;
};

#endif // SPRITE_EDITOR_H
