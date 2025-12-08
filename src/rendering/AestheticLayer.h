#ifndef AESTHETIC_LAYER_H
#define AESTHETIC_LAYER_H

#include <SDL.h>
#include <vector>
#include <cstdint>

class AestheticLayer {
public:
    // Define las dimensiones del framebuffer de la consola de fantasía.
    static constexpr int FRAMEBUFFER_WIDTH = 256;
    static constexpr int FRAMEBUFFER_HEIGHT = 256;

    AestheticLayer(SDL_Renderer* renderer);
    ~AestheticLayer();

    // Limpia el framebuffer con un índice de color de la paleta.
    void Clear(uint8_t colorIndex);

    // Dibuja un píxel en el framebuffer.
    void SetPixel(int x, int y, uint8_t colorIndex);

    // Dibuja una línea en el framebuffer usando el algoritmo de Bresenham.
    void Line(int x1, int y1, int x2, int y2, uint8_t colorIndex);

    // Dibuja el contorno de un rectángulo.
    void Rect(int x, int y, int w, int h, uint8_t colorIndex);

    // Dibuja un rectángulo relleno.
    void RectFill(int x, int y, int w, int h, uint8_t colorIndex);

    // Renderiza el framebuffer a la ventana principal.
    void Present();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> pixelBuffer; // Buffer de píxeles en formato ARGB8888 para la textura.
    std::vector<uint8_t> framebuffer;  // Buffer de índices de color (256x256).
    std::vector<SDL_Color> palette;    // Paleta de 16 colores.
};

#endif // AESTHETIC_LAYER_H