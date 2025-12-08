#include "AestheticLayer.h"
#include "rendering/EmbeddedFont.h"
#include <stdexcept>

AestheticLayer::AestheticLayer(SDL_Renderer* renderer) : renderer(renderer) {
    if (!renderer) {
        throw std::runtime_error("The renderer provided to AestheticLayer is null.");
    }

    // Create the texture that we will use as our final canvas.
    // SDL_TEXTUREACCESS_STREAMING allows us to update it efficiently every frame.
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                FRAMEBUFFER_WIDTH,
                                FRAMEBUFFER_HEIGHT);
    if (!texture) {
        throw std::runtime_error("Could not create the aesthetic layer texture.");
    }

    // Initialize the buffers.
    framebuffer.resize(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);
    pixelBuffer.resize(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);

    // Define the 16-color palette (PICO-8).
    palette = {
        {0, 0, 0, 255},       // 0: Black
        {29, 43, 83, 255},    // 1: Dark Blue
        {126, 37, 83, 255},   // 2: Dark Purple
        {0, 135, 81, 255},    // 3: Dark Green
        {171, 82, 54, 255},   // 4: Brown
        {95, 87, 79, 255},    // 5: Dark Gray
        {194, 195, 199, 255}, // 6: Light Gray
        {255, 241, 232, 255}, // 7: White
        {255, 0, 77, 255},    // 8: Red
        {255, 163, 0, 255},   // 9: Orange
        {255, 236, 39, 255},  // 10: Yellow
        {0, 228, 54, 255},    // 11: Green
        {41, 173, 255, 255},  // 12: Blue
        {131, 118, 156, 255}, // 13: Lavender
        {255, 119, 168, 255}, // 14: Pink
        {255, 204, 170, 255}  // 15: Light Peach
    };
}

AestheticLayer::~AestheticLayer() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void AestheticLayer::SetCamera(int x, int y) {
    cameraX = x;
    cameraY = y;
}

void AestheticLayer::SetTransparentColor(std::optional<uint8_t> colorIndex) {
    transparentColor = colorIndex;
}

void AestheticLayer::Clear(uint8_t colorIndex) {
    // Ensure the fill value is of the correct type (uint8_t) to avoid conversion warnings.
    uint8_t finalColorIndex = colorIndex % static_cast<uint8_t>(palette.size());
    std::fill(framebuffer.begin(), framebuffer.end(), finalColorIndex);
}

void AestheticLayer::SetPixel(int x, int y, uint8_t colorIndex) {
    // Check for transparent color before drawing.
    if (transparentColor.has_value() && colorIndex == transparentColor.value()) {
        return;
    }

    int screenX = x - cameraX;
    int screenY = y - cameraY;

    if (screenX >= 0 && screenX < FRAMEBUFFER_WIDTH && screenY >= 0 && screenY < FRAMEBUFFER_HEIGHT) {
        framebuffer[screenY * FRAMEBUFFER_WIDTH + screenX] = colorIndex % palette.size();
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
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    // Draw the 4 lines of the rectangle.
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

void AestheticLayer::Circ(int centerX, int centerY, int radius, uint8_t colorIndex) {
    if (radius < 0) return;
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SetPixel(centerX + x, centerY + y, colorIndex);
        SetPixel(centerX + y, centerY + x, colorIndex);
        SetPixel(centerX - y, centerY + x, colorIndex);
        SetPixel(centerX - x, centerY + y, colorIndex);
        SetPixel(centerX - x, centerY - y, colorIndex);
        SetPixel(centerX - y, centerY - x, colorIndex);
        SetPixel(centerX + y, centerY - x, colorIndex);
        SetPixel(centerX + x, centerY - y, colorIndex);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void AestheticLayer::CircFill(int centerX, int centerY, int radius, uint8_t colorIndex) {
    if (radius < 0) return;
    // Using a simpler, albeit less optimal, scanline method for clarity.
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx * dx + dy * dy <= radius * radius) {
                SetPixel(centerX + dx, centerY + dy, colorIndex);
            }
        }
    }
}

uint8_t AestheticLayer::Pget(int x, int y) {
    int screenX = x - cameraX;
    int screenY = y - cameraY;

    if (screenX >= 0 && screenX < FRAMEBUFFER_WIDTH && screenY >= 0 && screenY < FRAMEBUFFER_HEIGHT) {
        return framebuffer[screenY * FRAMEBUFFER_WIDTH + screenX];
    }
    return 0; // Return color 0 (black) for out-of-bounds pixels.
}

void AestheticLayer::Print(const std::string& text, int x, int y, uint8_t colorIndex) {
    int cursorX = x;
    for (const char& c : text) {
        // Only render printable ASCII characters.
        if (c >= 32 && c <= 126) {
            // Calculate the starting index for the character's glyph data.
            int charIndex = (c - 32) * EmbeddedFont::FONT_HEIGHT;

            // Iterate through the 8 rows of the character's glyph.
            for (int row = 0; row < EmbeddedFont::FONT_HEIGHT; ++row) {
                uint8_t rowData = EmbeddedFont::FONT_DATA[charIndex + row];

                // Iterate through the 8 bits of the row data (representing columns).
                for (int col = 0; col < EmbeddedFont::FONT_WIDTH; ++col) {
                    // If the bit is set, draw a pixel.
                    // We check the most significant bit for column 0, so we shift by (7 - col).
                    if ((rowData >> (7 - col)) & 1) {
                        SetPixel(cursorX + col, y + row, colorIndex);
                    }
                }
            }
        }
        // Advance the cursor for the next character.
        cursorX += EmbeddedFont::FONT_WIDTH; // Character spacing is now built into the font glyphs.
    }
}

void AestheticLayer::Present() {
    // 1. Translate our color-indexed framebuffer to a 32-bit pixel buffer.
    for (size_t i = 0; i < framebuffer.size(); ++i) {
        const auto& color = palette[framebuffer[i]];
        // ARGB8888 format
        pixelBuffer[i] = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    }

    // 2. Update the SDL texture with our pixel buffer.
    SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), FRAMEBUFFER_WIDTH * sizeof(uint32_t));

    // 3. Clear the renderer.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background for the letterbox bars
    SDL_RenderClear(renderer);

    // 4. Copy the texture to the renderer, scaling it to fit the window while maintaining aspect ratio.
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // 5. Show the result on the screen.
    SDL_RenderPresent(renderer);
}