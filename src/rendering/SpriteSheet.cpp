#include "rendering/SpriteSheet.h"
#include <SDL.h>
#include <iostream>
#include <algorithm>
#include <cmath>

// stb_image for PNG loading (v1.1.5)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

SpriteSheet::SpriteSheet() : loaded(false), tileSize(8) {
    data.resize(SHEET_WIDTH * SHEET_HEIGHT, 0);
}

SpriteSheet::~SpriteSheet() {
    // Nothing to clean up
}

bool SpriteSheet::LoadFromPNG(const std::string& path, int ts) {
    // Validate tile size
    if (ts != 8 && ts != 16) {
        std::cerr << "SpriteSheet: Invalid tile size " << ts << ". Must be 8 or 16." << std::endl;
        return false;
    }
    
    tileSize = ts;
    
    // Load image using stb_image (v1.1.5 - supports PNG, JPG, BMP, TGA, etc.)
    int width, height, channels;
    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 3);  // Force RGB
    
    if (!pixels) {
        std::cerr << "SpriteSheet: Failed to load image: " << path << std::endl;
        std::cerr << "stb_image error: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    // Validate dimensions
    if (width != SHEET_WIDTH || height != SHEET_HEIGHT) {
        std::cerr << "SpriteSheet: Invalid dimensions " << width << "x" << height 
                  << ". Must be 128x128." << std::endl;
        stbi_image_free(pixels);
        return false;
    }
    
    // For now, create a simple PICO-8 style palette for color matching
    std::vector<uint32_t> palette = {
        0x000000, // 0: Black
        0x1D2B53, // 1: Dark Blue
        0x7E2553, // 2: Dark Purple
        0x008751, // 3: Dark Green
        0xAB5236, // 4: Brown
        0x5F574F, // 5: Dark Gray
        0xC2C3C7, // 6: Light Gray
        0xFFF1E8, // 7: White
        0xFF004D, // 8: Red
        0xFFA300, // 9: Orange
        0xFFEC27, // 10: Yellow
        0x00E436, // 11: Green
        0x29ADFF, // 12: Blue
        0x83769C, // 13: Lavender
        0xFF77A8, // 14: Pink
        0xFFCCAA  // 15: Peach
    };
    
    // Convert pixels to palette indices
    for (int y = 0; y < SHEET_HEIGHT; y++) {
        for (int x = 0; x < SHEET_WIDTH; x++) {
            int pixelIndex = (y * width + x) * 3;  // RGB = 3 bytes per pixel
            uint8_t r = pixels[pixelIndex + 0];
            uint8_t g = pixels[pixelIndex + 1];
            uint8_t b = pixels[pixelIndex + 2];
            
            // Convert RGB to palette index
            uint8_t paletteIndex = RGBToPaletteIndex(r, g, b, palette);
            data[y * SHEET_WIDTH + x] = paletteIndex;
        }
    }
    
    // Free stb_image data
    stbi_image_free(pixels);
    
    loaded = true;
    std::cout << "SpriteSheet: Successfully loaded " << path 
              << " (" << GetSpriteCount() << " sprites, " 
              << tileSize << "x" << tileSize << " tiles)" << std::endl;
    
    return true;
}

bool SpriteSheet::GetSpriteData(int spriteId, uint8_t* outData) const {
    if (!loaded || !outData) return false;
    
    int spritesPerRow = SHEET_WIDTH / tileSize;
    int maxSprites = (SHEET_WIDTH / tileSize) * (SHEET_HEIGHT / tileSize);
    
    if (spriteId < 0 || spriteId >= maxSprites) {
        return false;
    }
    
    // Calculate sprite position in sheet
    int spriteX = (spriteId % spritesPerRow) * tileSize;
    int spriteY = (spriteId / spritesPerRow) * tileSize;
    
    // Copy sprite data
    for (int y = 0; y < tileSize; y++) {
        for (int x = 0; x < tileSize; x++) {
            int sheetX = spriteX + x;
            int sheetY = spriteY + y;
            outData[y * tileSize + x] = data[sheetY * SHEET_WIDTH + sheetX];
        }
    }
    
    return true;
}

uint8_t SpriteSheet::GetPixel(int x, int y) const {
    if (!loaded) return 0;
    if (x < 0 || x >= SHEET_WIDTH || y < 0 || y >= SHEET_HEIGHT) return 0;
    
    return data[y * SHEET_WIDTH + x];
}

int SpriteSheet::GetSpriteCount() const {
    return (SHEET_WIDTH / tileSize) * (SHEET_HEIGHT / tileSize);
}

uint8_t SpriteSheet::RGBToPaletteIndex(uint8_t r, uint8_t g, uint8_t b, const std::vector<uint32_t>& palette) const {
    int bestIndex = 0;
    int bestDistance = INT_MAX;
    
    for (size_t i = 0; i < palette.size(); i++) {
        uint32_t color = palette[i];
        int pr = (color >> 16) & 0xFF;
        int pg = (color >> 8) & 0xFF;
        int pb = color & 0xFF;
        
        // Euclidean distance in RGB space
        int dr = r - pr;
        int dg = g - pg;
        int db = b - pb;
        int distance = dr*dr + dg*dg + db*db;
        
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = static_cast<int>(i);
        }
    }
    
    return static_cast<uint8_t>(bestIndex);
}
