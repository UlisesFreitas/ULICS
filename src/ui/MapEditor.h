#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

// Forward declarations
class AestheticLayer;
class InputManager;
class SystemSprites;

/**
 * @brief Map Editor for ULICS Fantasy Console
 * 
 * T

ile-based map editor inspired by PICO-8, TIC-80, and Tiled.
 * Features:
 * - 128x64 tile grid (1024x512 pixels at 8x8 tile size)
 * - 8 independent layers (background to foreground)
 * - Tile picker from spritesheet (256 tiles)
 * - Drawing tools: Pencil, Fill, Eraser
 * - Layer visibility toggles
 * - Save/Load to JSON format
 */
class MapEditor {
public:
    MapEditor();
    ~MapEditor();
    
    // Core methods
    void Update(InputManager& input);
    void Render(AestheticLayer& renderer, InputManager& input);
    
    // Initialization
    void Initialize(const std::string& mapPath);
    
    // State queries
    bool IsActive() const { return isActive; }
    void SetActive(bool active, AestheticLayer* renderer = nullptr);
    
    // Set system sprites for icons
    void SetSystemSprites(SystemSprites* sprites) { systemSprites = sprites; }
    
    // Map manipulation
    void SetTile(int x, int y, int layer, uint8_t tileId);
    uint8_t GetTile(int x, int y, int layer) const;
    void ClearLayer(int layer);
    void ClearAllLayers();
    
    // Layer management
    void SetActiveLayer(int layer);
    int GetActiveLayer() const { return activeLayer; }
    void ToggleLayerVisibility(int layer);
    bool IsLayerVisible(int layer) const;
    void SetLayerName(int layer, const std::string& name);
    std::string GetLayerName(int layer) const;
    
    // File operations
    bool SaveToJSON(const std::string& path);
    bool LoadFromJSON(const std::string& path);
    
private:
    // Tool types
    enum class Tool {
        PENCIL,  // Draw single tiles
        FILL,    // Flood fill
        ERASER,  // Erase tiles (set to 0)
        PICKER   // Pick tile from map
    };
    
    // Map specifications (PICO-8/TIC-80 inspired)
    static constexpr int MAP_WIDTH = 128;   // Tiles wide
    static constexpr int MAP_HEIGHT = 64;   // Tiles high
    static constexpr int TILE_SIZE = 8;     // 8x8 pixels
    static constexpr int LAYER_COUNT = 8;   // 8 layers
    static constexpr int TILES_PER_LAYER = MAP_WIDTH * MAP_HEIGHT;  // 8192
    
    // Layer data structure
    struct Layer {
        std::string name;
        bool visible;
        uint8_t opacity;  // 0-100 (for future use)
        std::vector<uint8_t> data;  // Tile IDs (0 = transparent)
        
        Layer() : name("Layer"), visible(true), opacity(100) {
            data.resize(TILES_PER_LAYER, 0);  // Initialize with empty tiles
        }
    };
    
    // Map state
    std::vector<Layer> layers;
    int activeLayer;
    std::string mapPath;
    
    // Tool state
    Tool currentTool;
    uint8_t selectedTile;  // Currently selected tile ID (0-255)
    
    // Continuous drawing state
    bool isDrawing;      // True while left mouse is held
    int lastDrawnTileX;  // Last tile painted (to avoid double-paint)
    int lastDrawnTileY;
    
    // UI state
    bool isActive;
    bool showGrid;
    
    // Camera and zoom (PICO-8 style)
    int cameraX;  // Camera offset in tiles
    int cameraY;
    float zoom;   // Zoom level (0.25 = 25%, 1.0 = 100%, 4.0 = 400%)
    
    // Pan state (middle mouse button)
    bool isPanning;
    int panStartX;
    int panStartY;
    int cameraStartX;
    int cameraStartY;
    
    // Layer sidebar state
    bool showLayerSidebar;
    int hoveredLayer;  // -1 = none, 0-7 = layer index
    
    // Toast/Feedback message system
    std::string toastMessage;
    int toastTimer;  // Frames remaining to show toast (0 = hide)
    static constexpr int TOAST_DURATION = 120;  // 2 seconds at 60fps
    
    // UI Layout constants (PICO-8 style - vertical layout, calculated from BOTTOM UP)
    static constexpr int TITLE_BAR_H = 10;
    static constexpr int STATUS_BAR_H = 10;
    
    // Spritesheet (BOTTOM, just above footer, 16x4 grid, FULL WIDTH)
    static constexpr int SHEET_COLS = 16;
    static constexpr int SHEET_ROWS = 4;  // 64 tiles per tab
    static constexpr int SHEET_SPRITE_SIZE = 16;  // 16x16 display per sprite
    static constexpr int SHEET_W = SHEET_COLS * SHEET_SPRITE_SIZE;  // 256px (full width)
    static constexpr int SHEET_H = SHEET_ROWS * SHEET_SPRITE_SIZE;  // 64px
    static constexpr int SHEET_X = 0;  // Full width, no margin
    static constexpr int SHEET_Y = 256 - STATUS_BAR_H - SHEET_H;  // 256 - 10 - 64 = 182
    
    // Toolbar (just above spritesheet, contains tool icons + tabs)
    static constexpr int TOOLBAR_H = 20;
    static constexpr int TOOLBAR_Y = SHEET_Y - TOOLBAR_H;  // 182 - 20 = 162
    
    // Map Viewport (fills remaining space from title bar to toolbar, FULL WIDTH with border)
    static constexpr int MAP_BORDER_X = 0;      // Border starts at x=0 (full width)
    static constexpr int MAP_BORDER_Y = TITLE_BAR_H;  // Border starts after title
    static constexpr int MAP_BORDER_W = 256;    // Border is full width
    static constexpr int MAP_BORDER_H = TOOLBAR_Y - TITLE_BAR_H;  // Border height
    static constexpr int MAP_X = MAP_BORDER_X + 1;  // Map inside border (+1px)
    static constexpr int MAP_Y = MAP_BORDER_Y + 1;  // Map inside border (+1px)
    static constexpr int MAP_W = MAP_BORDER_W - 2;  // Map width (254px)
    static constexpr int MAP_H = MAP_BORDER_H - 2;  // Map height
    
    // Layer Sidebar (left side, toggleable)
    static constexpr int SIDEBAR_W = 60;  // Width when open (wider for "Show"/"Hide")
    static constexpr int SIDEBAR_X = 0;
    static constexpr int SIDEBAR_Y = MAP_Y;
    static constexpr int SIDEBAR_H = MAP_H;
    static constexpr int LAYER_ITEM_H = 16;  // Height per layer item
    
    // Sidebar UI element positions
    static constexpr int SIDEBAR_TITLE_Y = 2;
    static constexpr int SIDEBAR_LIST_START_Y = 12;  // Start of layer list
    static constexpr int LAYER_NUM_X = 4;   // Layer number position
    static constexpr int LAYER_VIS_X = 14;  // "Show"/"Hide" text position
    static constexpr int LAYER_VIS_W = 24;  // Width of visibility click area
    static constexpr int LAYER_ARROW_X = 50;  // Active arrow position
    
    // Toggle button (in title bar)
    static constexpr int TOGGLE_BTN_X = 2;
    static constexpr int TOGGLE_BTN_Y = 1;
    static constexpr int TOGGLE_BTN_SIZE = 8;
    
    // Tab system (4 tabs for 256 tiles total, RIGHT EDGE of screen)
    static constexpr int SPRITES_PER_TAB = 64;  // 16x4 grid
    static constexpr int TAB_COUNT = 4;  // 4 tabs * 64 = 256 tiles
    static constexpr int TAB_BUTTON_SIZE = 16;
    static constexpr int TAB_SELECTOR_X = 256 - (TAB_COUNT * TAB_BUTTON_SIZE);  // End at x=256
    static constexpr int TAB_SELECTOR_Y = TOOLBAR_Y + 3;  // +3 instead of +2 (1px lower)
    int currentTab;  // 0-3
    
    // Layer selector (toolbar area)
    static constexpr int LAYER_BUTTON_SIZE = 16;
    
    // System sprites for icons
    SystemSprites* systemSprites;
    
    // Rendering methods
    void RenderTitleBar(AestheticLayer& renderer);
    void RenderMapViewport(AestheticLayer& renderer);
    void RenderToolbar(AestheticLayer& renderer);
    void RenderSpritesheet(AestheticLayer& renderer);
    void RenderStatusBar(AestheticLayer& renderer);
    void RenderGrid(AestheticLayer& renderer);
    void RenderLayerSidebar(AestheticLayer& renderer);  // New sidebar
    
    // Input handling
    void HandleViewportClick(int mouseX, int mouseY);
    void HandlePickerClick(int mouseX, int mouseY);
    void HandleToolbarClick(int mouseX, int mouseY);
    void HandleLayerClick(int mouseX, int mouseY);
    void HandleKeyboard(InputManager& input);
    
    // Tool implementations
    void UsePencil(int tileX, int tileY);
    void UseFill(int tileX, int tileY, uint8_t targetTile, uint8_t replacementTile);
    void UseEraser(int tileX, int tileY);
    void UsePicker(int tileX, int tileY);
    
    // Helper methods
    bool IsValidTileCoord(int x, int y) const;
    int ScreenToTileX(int screenX) const;
    int ScreenToTileY(int screenY) const;
    int TileToScreenX(int tileX) const;
    int TileToScreenY(int tileY) const;
    
    // Map drawing helpers (reduce code duplication)
    int GetMapDrawX() const;
    int GetMapDrawY() const;
    int GetMapPixelWidth() const;
    int GetMapPixelHeight() const;
    float GetTileSize() const;
    
    // Flood fill helper (recursive)
    void FloodFillRecursive(int x, int y, int layer, uint8_t targetTile, uint8_t replacementTile);
    
    // Toast message helper
    void ShowToast(const std::string& message);
    
    // Debug logging
    void Log(const std::string& message);
};

#endif // MAP_EDITOR_H
