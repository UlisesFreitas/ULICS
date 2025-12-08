#include "AestheticLayer.h"
#include <stdexcept>

AestheticLayer::AestheticLayer(SDL_Renderer* renderer) : renderer(renderer) {
    if (!renderer) {
        throw std::runtime_error("El renderer proporcionado a AestheticLayer es nulo.");
    }

    // Crea la textura que usaremos como nuestro lienzo final.
    // SDL_TEXTUREACCESS_STREAMING nos permite actualizarla eficientemente cada fotograma.
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                FRAMEBUFFER_WIDTH,
                                FRAMEBUFFER_HEIGHT);
    if (!texture) {
        throw std::runtime_error("No se pudo crear la textura de la capa estética.");
    }

    // Inicializa los buffers.
    framebuffer.resize(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);
    pixelBuffer.resize(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);

    // Define la paleta de 16 colores (PICO-8).
    palette = {
        {0, 0, 0, 255},       // 0: Negro
        {29, 43, 83, 255},    // 1: Azul oscuro
        {126, 37, 83, 255},   // 2: Púrpura oscuro
        {0, 135, 81, 255},    // 3: Verde oscuro
        {171, 82, 54, 255},   // 4: Marrón
        {95, 87, 79, 255},    // 5: Gris oscuro
        {194, 195, 199, 255}, // 6: Gris claro
        {255, 241, 232, 255}, // 7: Blanco
        {255, 0, 77, 255},    // 8: Rojo
        {255, 163, 0, 255},   // 9: Naranja
        {255, 236, 39, 255},  // 10: Amarillo
        {0, 228, 54, 255},    // 11: Verde
        {41, 173, 255, 255},  // 12: Azul
        {131, 118, 156, 255}, // 13: Lavanda
        {255, 119, 168, 255}, // 14: Rosa
        {255, 204, 170, 255}  // 15: Melocotón claro
    };
}

AestheticLayer::~AestheticLayer() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void AestheticLayer::Clear(uint8_t colorIndex) {
    // Aseguramos que el valor a rellenar sea del tipo correcto (uint8_t) para evitar warnings de conversión.
    uint8_t finalColorIndex = colorIndex % static_cast<uint8_t>(palette.size());
    std::fill(framebuffer.begin(), framebuffer.end(), finalColorIndex);
}

void AestheticLayer::SetPixel(int x, int y, uint8_t colorIndex) {
    if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
        framebuffer[y * FRAMEBUFFER_WIDTH + x] = colorIndex % palette.size();
    }
}

void AestheticLayer::Line(int x1, int y1, int x2, int y2, uint8_t colorIndex) {
    int dx = std::abs(x2 - x1);
    int dy = -std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        SetPixel(x1, y1, colorIndex);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void AestheticLayer::Rect(int x, int y, int w, int h, uint8_t colorIndex) {
    if (w <= 0 || h <= 0) return;
    // Aseguramos no dibujar fuera de los límites para las líneas horizontales y verticales
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    // Dibujamos las 4 líneas del rectángulo
    for (int i = x; i <= x2; ++i) {
        SetPixel(i, y, colorIndex);
        SetPixel(i, y2, colorIndex);
    }
    for (int i = y + 1; i < y2; ++i) {
        SetPixel(x, i, colorIndex);
        SetPixel(x2, i, colorIndex);
    }
}

void AestheticLayer::RectFill(int x, int y, int w, int h, uint8_t colorIndex) {
    int x2 = x + w;
    int y2 = y + h;
    for (int j = y; j < y2; ++j) {
        for (int i = x; i < x2; ++i) {
            SetPixel(i, j, colorIndex);
        }
    }
}

void AestheticLayer::Present() {
    // 1. Traducir nuestro framebuffer de índices de color a un buffer de píxeles de 32 bits.
    for (size_t i = 0; i < framebuffer.size(); ++i) {
        const auto& color = palette[framebuffer[i]];
        // Formato ARGB8888
        pixelBuffer[i] = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    }

    // 2. Actualizar la textura de SDL con nuestro buffer de píxeles.
    SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), FRAMEBUFFER_WIDTH * sizeof(uint32_t));

    // 3. Limpiar el renderer.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fondo negro para las barras laterales
    SDL_RenderClear(renderer);

    // 4. Copiar la textura al renderer, escalándola para que se ajuste a la ventana manteniendo la relación de aspecto.
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // 5. Mostrar el resultado en la pantalla.
    SDL_RenderPresent(renderer);
}