#include "rendering/Map.h"
#include <fstream>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

Map::Map() : loaded(false), width(0), height(0), tileSize(8) {
}

Map::~Map() {
}

bool Map::LoadFromJSON(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Map: Failed to open " << path << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        // Parse map dimensions
        width = j.value("width", 32);
        height = j.value("height", 32);
        tileSize = j.value("tile_size", 8);

        // Parse layers
        if (j.contains("layers") && j["layers"].is_array()) {
            for (const auto& layerJson : j["layers"]) {
                Layer layer;
                layer.id = layerJson.value("id", 0);
                layer.name = layerJson.value("name", "");
                layer.visible = layerJson.value("visible", true);

                // Load tile data
                if (layerJson.contains("data") && layerJson["data"].is_array()) {
                    const auto& dataArray = layerJson["data"];
                    layer.data.reserve(width * height);
                    
                    for (const auto& tile : dataArray) {
                        layer.data.push_back(static_cast<uint8_t>(tile.get<int>()));
                    }

                    // Ensure correct size
                    layer.data.resize(width * height, 0);
                }

                layers.push_back(layer);
            }
        }

        loaded = true;
        std::cout << "Map: Loaded " << path << " (" << width << "x" << height 
                  << ", " << layers.size() << " layers)" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Map: JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

uint8_t Map::GetTile(int x, int y, int layer) const {
    if (!loaded || layer < 0 || layer >= static_cast<int>(layers.size())) {
        return 0;
    }

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }

    const auto& layerData = layers[layer].data;
    int index = y * width + x;
    
    if (index >= 0 && index < static_cast<int>(layerData.size())) {
        return layerData[index];
    }

    return 0;
}

void Map::SetTile(int x, int y, uint8_t tileId, int layer) {
    if (!loaded || layer < 0 || layer >= static_cast<int>(layers.size())) {
        return;
    }

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    auto& layerData = layers[layer].data;
    int index = y * width + x;
    
    if (index >= 0 && index < static_cast<int>(layerData.size())) {
        layerData[index] = tileId;
    }
}
