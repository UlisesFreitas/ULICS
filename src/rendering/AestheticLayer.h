#ifndef AESTHETIC_LAYER_H
#define AESTHETIC_LAYER_H

#include <SDL.h>
#include <vector>
#include <cstdint>

class AestheticLayer {
public:
    // Defines the fantasy console's framebuffer dimensions.
    static constexpr int FRAMEBUFFER_WIDTH = 256;
    static constexpr int FRAMEBUFFER_HEIGHT = 256;

    AestheticLayer(SDL_Renderer* renderer);
    ~AestheticLayer();

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

    // Renders the framebuffer to the main window.
    void Present();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> pixelBuffer; // Pixel buffer in ARGB8888 format for the texture.
    std::vector<uint8_t> framebuffer;  // Color index buffer (256x256).
    std::vector<SDL_Color> palette;    // 16-color palette.
};

#endif // AESTHETIC_LAYER_H