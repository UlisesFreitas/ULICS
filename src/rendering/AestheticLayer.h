#ifndef AESTHETIC_LAYER_H
#define AESTHETIC_LAYER_H

#include <SDL.h>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include "rendering/SpriteSheet.h"

class AestheticLayer {
public:
    // Defines the fantasy console's framebuffer dimensions.
    static constexpr int FRAMEBUFFER_WIDTH = 256;
    static constexpr int FRAMEBUFFER_HEIGHT = 256;

    AestheticLayer(SDL_Renderer* renderer);
    ~AestheticLayer();

    // Sets the camera offset for all subsequent drawing operations.
    void SetCamera(int x, int y);

    // Clears the framebuffer with a palette color index.
    void Clear(uint8_t colorIndex);

    // Draws a pixel on the framebuffer.
    void SetPixel(int x, int y, uint8_t colorIndex);

    // Draws a line on the framebuffer using Bresenham's algorithm.
    void Line(int x1, int y1, int x2, int y2, uint8_t colorIndex);

    // Draws the outline of a rectangle.
    void Rect(int x, int y, int w, int h, uint8_t colorIndex);

    // Draws a filled rectangle.
    void RectFill(int x, int y, int w, int h, uint8_t colorIndex);

    // Draws the outline of a circle.
    void Circ(int centerX, int centerY, int radius, uint8_t colorIndex);

    // Draws a filled circle.
    void CircFill(int centerX, int centerY, int radius, uint8_t colorIndex);

    // Gets the color index of a pixel at the given coordinates.
    uint8_t Pget(int x, int y);

    // Draws text on the framebuffer using the embedded font.
    void Print(const std::string& text, int x, int y, uint8_t colorIndex);

    // === Sprite Rendering (Phase 5) ===
    // Draws a sprite from the loaded sprite sheet.
    // Parameters: spriteId, x, y, width (in tiles), height (in tiles), flipX, flipY
    void DrawSprite(int spriteId, int x, int y, int w = 1, int h = 1, bool flipX = false, bool flipY = false);
    
    // Draws a section of the sprite sheet (advanced sprite rendering).
    // Parameters: source x, y, width, height, destination x, y, dest width, dest height
    void DrawSpriteSection(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);

    // === Map Rendering (Phase 5.8) ===
    // Draws a portion of the map
    // Parameters: map x/y (tiles), screen x/y (pixels), width/height (tiles), layer mask
    void DrawMap(class Map* map, int mx, int my, int sx, int sy, int w, int h, uint8_t layerMask = 0xFF);

    // Sets the transparent color index for sprite operations.
    void SetTransparentColor(int colorIndex);

    // === Palette Management ===
    // Sets the palette size (must be 16, 32, 64, or 128).
    void SetPaletteSize(int size);

    // Loads a complete custom palette.
    void LoadPalette(const std::vector<SDL_Color>& newPalette);

    // Sets an individual color in the palette.
    void SetPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b);

    // Gets the current palette size.
    int GetPaletteSize() const;

    // Resets the palette to the default PICO-8 16-color palette.
    void ResetToDefaultPalette();

    // Renders the framebuffer to the main window.
    void Present();
    
    // Captures the current frame as a PNG screenshot (v1.5.3)
    void CaptureScreenshot();
    
    // Get pixel data for GIF recording (v1.5.4)
    const uint8_t* GetPixelData() const;
    int GetPixelDataSize() const { return FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 4; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> pixelBuffer; // Pixel buffer in ARGB8888 format for the texture.
    std::vector<uint8_t> framebuffer;  // Color index buffer (256x256).
    std::vector<SDL_Color> palette;    // 16-color palette.
    int cameraX = 0;
    int cameraY = 0;
    int transparentColorIndex = -1; // -1 means no transparent color set.
    
    // Sprite rendering (Phase 5)
    std::unique_ptr<SpriteSheet> spriteSheet;
};

#endif // AESTHETIC_LAYER_H