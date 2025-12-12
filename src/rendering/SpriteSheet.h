#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief SpriteSheet class - Loads and manages a 128x128 sprite sheet
 * 
 * Loads sprites from PNG files and converts them to palette-indexed format.
 * Supports 8x8 tile mode (256 sprites) or 16x16 tile mode (64 sprites).
 */
class SpriteSheet {
public:
    SpriteSheet();
    ~SpriteSheet();

    /**
     * @brief Load sprite sheet from PNG file
     * @param path Path to PNG file (128x128 pixels)
     * @param tileSize Size of each tile (8 or 16)
     * @return true if loaded successfully, false otherwise
     */
    bool LoadFromPNG(const std::string& path, int tileSize = 8);

    /**
     * @brief Get pixel data for a specific sprite
     * @param spriteId Sprite index (0-255 for 8x8, 0-63 for 16x16)
     * @param outData Output buffer (must be tileSize*tileSize bytes)
     * @return true if sprite exists, false otherwise
     */
    bool GetSpriteData(int spriteId, uint8_t* outData) const;

    /**
     * @brief Get pixel at specific position in sprite sheet
     * @param x X coordinate (0-127)
     * @param y Y coordinate (0-127)
     * @return Palette index (0-255)
     */
    uint8_t GetPixel(int x, int y) const;

    /**
     * @brief Check if sprite sheet is loaded
     */
    bool IsLoaded() const { return loaded; }

    /**
     * @brief Get tile size (8 or 16)
     */
    int GetTileSize() const { return tileSize; }

    /**
     * @brief Get total number of sprites
     */
    int GetSpriteCount() const;

    /**
     * @brief Get sprite sheet dimensions
     */
    static constexpr int SHEET_WIDTH = 128;
    static constexpr int SHEET_HEIGHT = 128;

private:
    /**
     * @brief Convert RGB color to nearest palette index
     * @param r Red (0-255)
     * @param g Green (0-255)
     * @param b Blue (0-255)
     * @param palette Reference to ULICS palette
     * @return Nearest palette index
     */
    uint8_t RGBToPaletteIndex(uint8_t r, uint8_t g, uint8_t b, const std::vector<uint32_t>& palette) const;

    bool loaded;
    int tileSize;  // 8 or 16
    std::vector<uint8_t> data;  // 128x128 array of palette indices
};

#endif // SPRITESHEET_H
