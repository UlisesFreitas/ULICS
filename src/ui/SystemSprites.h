#ifndef SYSTEM_SPRITES_H
#define SYSTEM_SPRITES_H

#include <cstdint>

class AestheticLayer;

/**
 * @brief System-wide sprite icons for UI elements
 * 
 * Provides a set of 8x8 icons for tools, buttons, and UI elements.
 * Icons are embedded directly in the code (like SystemScripts).
 * 
 * Icons use the standard 16-color palette.
 */
class SystemSprites {
public:
    SystemSprites();
    ~SystemSprites();
    
    // Initialize sprites (loads embedded defaults)
    void Initialize();
    
    // Draw a sprite at position (x,y)
    void DrawSprite(AestheticLayer& renderer, int spriteId, int x, int y, int scale = 1);
    
    // Icon IDs (first 16 sprites reserved for UI)
    enum class Icon : int {
        PENCIL = 0,
        FILL = 1,
        LINE = 2,
        RECT = 3,
        PICKER = 4,
        SAVE = 5,
        LOAD = 6,
        PLAY = 7,
        STOP = 8,
        ARROW_LEFT = 9,
        ARROW_RIGHT = 10,
        ARROW_UP = 11,
        ARROW_DOWN = 12,
        FOLDER = 13,
        FILE = 14,
        GEAR = 15,
        // NEW: Sprite Editor utility icons
        GRID = 16,
        FILLED_RECT = 17,
        FLIP_H = 18,        // Horizontal flip
        FLIP_V = 19,        // Vertical flip
        CLEAR = 20,         // Clear/trash icon
        COPY = 21,          // Copy icon (2 overlapping squares)
        PASTE = 22,         // Paste icon (clipboard)
        
        // Rotate icons (23-24)
        ROTATE_LEFT = 23,   // Rotate 90° counter-clockwise
        ROTATE_RIGHT = 24   // Rotate 90° clockwise
    };
    
private:
    // Sprite data: 256 sprites of 8x8 pixels, palette indices
    uint8_t sprites[256][8][8];
    
    // Generate default icons procedurally
    void GenerateDefaultIcons();
    
    // Individual icon generators
    void GeneratePencilIcon();
    void GenerateFillIcon();
    void GenerateLineIcon();
    void GenerateRectIcon();
    void GeneratePickerIcon();
    void GenerateArrowIcons();
    void GenerateSaveIcon();
    void GenerateLoadIcon();
    void GeneratePlayStopIcons();
    void GenerateFolderFileIcons();
    void GenerateGearIcon();
    
    // NEW: Utility icon generators
    void GenerateUtilityIcons();  // Generates all utility icons (16-22)
    void GenerateRotateIcons();   // Generates rotate icons (23-24)
};

#endif // SYSTEM_SPRITES_H
