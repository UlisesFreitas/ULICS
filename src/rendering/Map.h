#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Map class - Manages tile-based map data with multiple layers
 */
class Map {
public:
    Map();
    ~Map();

    /**
     * @brief Load map from JSON file
     * @param path Path to map.json
     * @return true if loaded successfully
     */
    bool LoadFromJSON(const std::string& path);

    /**
     * @brief Get tile ID at specific position and layer
     * @param x X coordinate in tiles
     * @param y Y coordinate in tiles
     * @param layer Layer index (0-7)
     * @return Tile/sprite ID (0-255)
     */
    uint8_t GetTile(int x, int y, int layer = 0) const;

    /**
     * @brief Set tile ID at specific position and layer
     * @param x X coordinate in tiles
     * @param y Y coordinate in tiles
     * @param tileId Sprite ID to set
     * @param layer Layer index (0-7)
     */
    void SetTile(int x, int y, uint8_t tileId, int layer = 0);

    /**
     * @brief Get map dimensions
     */
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetTileSize() const { return tileSize; }
    int GetLayerCount() const { return static_cast<int>(layers.size()); }

    /**
     * @brief Check if loaded
     */
    bool IsLoaded() const { return loaded; }

private:
    struct Layer {
        int id;
        std::string name;
        bool visible;
        std::vector<uint8_t> data;  // Flat array: data[y * width + x]
    };

    bool loaded;
    int width;
    int height;
    int tileSize;
    std::vector<Layer> layers;
};

#endif // MAP_H
