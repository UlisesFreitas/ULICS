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
        GEAR = 15
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
};

#endif // SYSTEM_SPRITES_H
