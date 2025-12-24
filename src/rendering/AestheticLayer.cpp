#include "AestheticLayer.h"
#include "rendering/EmbeddedFont.h"
#include "rendering/Map.h"
#include "capture/Screenshot.h"  // For screenshot capture (v1.5.3)
#include <stdexcept>
#include <iostream>  // For std::cout, std::cerr

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

    // Define the default 32-color palette (PICO-8 + TIC-80 extended).
    palette = {
        // First 16 colors (PICO-8 compatible)
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
        {255, 204, 170, 255}, // 15: Light Peach
        
        // Extended 16 colors (TIC-80 style)
        {26, 28, 44, 255},    // 16: Very dark blue
        {93, 39, 93, 255},    // 17: Dark purple 2
        {177, 62, 83, 255},   // 18: Red-purple
        {239, 125, 87, 255},  // 19: Light orange
        {255, 205, 117, 255}, // 20: Light yellow
        {167, 240, 112, 255}, // 21: Light green
        {56, 183, 100, 255},  // 22: Medium green
        {37, 113, 121, 255},  // 23: Teal
        {41, 54, 111, 255},   // 24: Navy
        {59, 93, 201, 255},   // 25: Royal blue
        {65, 166, 246, 255},  // 26: Sky blue
        {115, 239, 247, 255}, // 27: Cyan
        {244, 244, 244, 255}, // 28: Off-white
        {148, 176, 194, 255}, // 29: Light blue-gray
        {86, 108, 134, 255},  // 30: Blue-gray
        {51, 60, 87, 255}     // 31: Dark blue-gray
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

void AestheticLayer::SetTransparentColor(int colorIndex) {
    transparentColorIndex = colorIndex;
}

// === Palette Management Implementation ===

void AestheticLayer::SetPaletteSize(int size) {
    // Validate that the size is one of the allowed values.
    if (size != 16 && size != 32 && size != 64 && size != 128) {
        throw std::invalid_argument("Palette size must be 16, 32, 64, or 128.");
    }

    // Resize the palette, keeping existing colors and filling new slots with black.
    int oldSize = static_cast<int>(palette.size());
    palette.resize(size);

    // Fill new slots with black if we expanded.
    for (int i = oldSize; i < size; ++i) {
        palette[i] = {0, 0, 0, 255};
    }
}

void AestheticLayer::LoadPalette(const std::vector<SDL_Color>& newPalette) {
    if (newPalette.empty()) {
        throw std::invalid_argument("Cannot load an empty palette.");
    }

    int size = static_cast<int>(newPalette.size());
    if (size != 16 && size != 32 && size != 64 && size != 128) {
        throw std::invalid_argument("Palette size must be 16, 32, 64, or 128.");
    }

    palette = newPalette;
}

void AestheticLayer::SetPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < 0 || index >= static_cast<int>(palette.size())) {
        throw std::out_of_range("Palette index out of range.");
    }

    palette[index] = {r, g, b, 255};
}

int AestheticLayer::GetPaletteSize() const {
    return static_cast<int>(palette.size());
}

SDL_Color AestheticLayer::GetPaletteColor(int index) const {
    if (index < 0 || index >= static_cast<int>(palette.size())) {
        return {0, 0, 0, 255};  // Return black for out of range
    }
    return palette[index];
}

void AestheticLayer::ResetToDefaultPalette() {
    // Reset to the default 32-color palette (PICO-8 + TIC-80 extended).
    palette = {
        // First 16 colors (PICO-8 compatible)
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
        {255, 204, 170, 255}, // 15: Light Peach
        
        // Extended 16 colors (TIC-80 style)
        {26, 28, 44, 255},    // 16: Very dark blue
        {93, 39, 93, 255},    // 17: Dark purple 2
        {177, 62, 83, 255},   // 18: Red-purple
        {239, 125, 87, 255},  // 19: Light orange
        {255, 205, 117, 255}, // 20: Light yellow
        {167, 240, 112, 255}, // 21: Light green
        {56, 183, 100, 255},  // 22: Medium green
        {37, 113, 121, 255},  // 23: Teal
        {41, 54, 111, 255},   // 24: Navy
        {59, 93, 201, 255},   // 25: Royal blue
        {65, 166, 246, 255},  // 26: Sky blue
        {115, 239, 247, 255}, // 27: Cyan
        {244, 244, 244, 255}, // 28: Off-white
        {148, 176, 194, 255}, // 29: Light blue-gray
        {86, 108, 134, 255},  // 30: Blue-gray
        {51, 60, 87, 255}     // 31: Dark blue-gray
    };
}

// === Sprite Sheet Management Implementation ===

bool AestheticLayer::LoadSpriteSheet(const std::string& path, int tileSize) {
    std::cout << "[AestheticLayer] Loading sprite sheet from: " << path << std::endl;
    
    // Create new sprite sheet if it doesn't exist
    if (!spriteSheet) {
        spriteSheet = std::make_unique<SpriteSheet>();
    }
    
    // Try to load the sprite sheet
    if (spriteSheet->LoadFromPNG(path, tileSize)) {
        loadedSpriteSheetPath = path;
        std::cout << "[AestheticLayer] Sprite sheet loaded successfully!" << std::endl;
        return true;
    } else {
        std::cerr << "[AestheticLayer] Failed to load sprite sheet from: " << path << std::endl;
        return false;
    }
}

bool AestheticLayer::ReloadSpriteSheet() {
    if (loadedSpriteSheetPath.empty()) {
        std::cerr << "[AestheticLayer] No sprite sheet path to reload" << std::endl;
        return false;
    }
    
    std::cout << "[AestheticLayer] Reloading sprite sheet from: " << loadedSpriteSheetPath << std::endl;
    return LoadSpriteSheet(loadedSpriteSheetPath);
}


void AestheticLayer::Clear(uint8_t colorIndex) {
    // Ensure the fill value is of the correct type (uint8_t) to avoid conversion warnings.
    uint8_t finalColorIndex = colorIndex % static_cast<uint8_t>(palette.size());
    std::fill(framebuffer.begin(), framebuffer.end(), finalColorIndex);
}

void AestheticLayer::SetPixel(int x, int y, uint8_t colorIndex) {
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
    x -= cameraX;
    y -= cameraY;

    if (w <= 0 || h <= 0) return;
    // Ensure we don't draw outside the bounds for horizontal and vertical lines.
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    // Draw the 4 lines of the rectangle.
    for (int i = x; i <= x2; ++i) {
        // Manually call SetPixel's logic to avoid double-offsetting
        if (i >= 0 && i < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) framebuffer[y * FRAMEBUFFER_WIDTH + i] = colorIndex;
        if (i >= 0 && i < FRAMEBUFFER_WIDTH && y2 >= 0 && y2 < FRAMEBUFFER_HEIGHT) framebuffer[y2 * FRAMEBUFFER_WIDTH + i] = colorIndex;
    }
    for (int i = y + 1; i < y2; ++i) {
        if (x >= 0 && x < FRAMEBUFFER_WIDTH && i >= 0 && i < FRAMEBUFFER_HEIGHT) framebuffer[i * FRAMEBUFFER_WIDTH + x] = colorIndex;
        if (x2 >= 0 && x2 < FRAMEBUFFER_WIDTH && i >= 0 && i < FRAMEBUFFER_HEIGHT) framebuffer[i * FRAMEBUFFER_WIDTH + x2] = colorIndex;
    }
}

void AestheticLayer::RectFill(int x, int y, int w, int h, uint8_t colorIndex) {
    x -= cameraX;
    y -= cameraY;

    int x2 = x + w;
    int y2 = y + h;

    // Clip the rectangle to the screen boundaries
    int startX = std::max(x, 0);
    int startY = std::max(y, 0);
    int endX = std::min(x2, FRAMEBUFFER_WIDTH);
    int endY = std::min(y2, FRAMEBUFFER_HEIGHT);

    for (int j = startY; j < endY; ++j) {
        for (int i = startX; i < endX; ++i) {
            framebuffer[j * FRAMEBUFFER_WIDTH + i] = colorIndex;
        }
    }
}

void AestheticLayer::RectFillRGB(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
    // Convert RGB to ARGB8888
    uint32_t argb = (0xFF << 24) | (r << 16) | (g << 8) | b;
    
    // Calculate bounds
    int startX = std::max(0, x);
    int startY = std::max(0, y);
    int endX = std::min(FRAMEBUFFER_WIDTH, x + width);
    int endY = std::min(FRAMEBUFFER_HEIGHT, y + height);
    
    // Write directly to pixelBuffer, bypassing framebuffer and palette
    for (int j = startY; j < endY; ++j) {
        for (int i = startX; i < endX; ++i) {
            pixelBuffer[j * FRAMEBUFFER_WIDTH + i] = argb;
            // Mark as RGB pixel (255 = skip in Present())
            framebuffer[j * FRAMEBUFFER_WIDTH + i] = 255;
        }
    }
}

// === RGB Drawing Functions (for fixed-color UI) ===

void AestheticLayer::PrintRGB(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!text) return;
    
    int cursorX = x;
    int cursorY = y;
    
    while (*text) {
        char c = *text++;
        
        // Handle newline
        if (c == '\n') {
            cursorX = x;
            cursorY += 8;  // Font height is typically 8
            continue;
        }
        
        // Only render printable ASCII characters
        if (c >= 32 && c <= 126) {
            // Calculate the starting index for the character's glyph data
            int charIndex = (c - 32) * EmbeddedFont::FONT_HEIGHT;
            
            // Iterate through the 8 rows of the character's glyph
            for (int row = 0; row < EmbeddedFont::FONT_HEIGHT; ++row) {
                uint8_t rowData = EmbeddedFont::FONT_DATA[charIndex + row];
                
                // Iterate through the 8 bits of the row data (representing columns)
                for (int col = 0; col < EmbeddedFont::FONT_WIDTH; ++col) {
                    // If the bit is set, draw a pixel
                    if (rowData & (1 << (7 - col))) {
                        RectFillRGB(cursorX + col, cursorY + row, 1, 1, r, g, b);
                    }
                }
            }
        }
        
        // Move cursor to next character position
        cursorX += EmbeddedFont::FONT_WIDTH;
    }
}

void AestheticLayer::LineRGB(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
    // Bresenham's line algorithm with RGB
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        RectFillRGB(x0, y0, 1, 1, r, g, b);  // Draw pixel
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void AestheticLayer::RectRGB(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
    // Draw rectangle outline with RGB
    LineRGB(x, y, x + width - 1, y, r, g, b);                           // Top
    LineRGB(x + width - 1, y, x + width - 1, y + height - 1, r, g, b); // Right
    LineRGB(x + width - 1, y + height - 1, x, y + height - 1, r, g, b); // Bottom
    LineRGB(x, y + height - 1, x, y, r, g, b);                          // Left
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
    int cursorX = x - cameraX;
    int cursorY = y - cameraY;

    for (char c : text) {
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
                        if (cursorX + col >= 0 && cursorX + col < FRAMEBUFFER_WIDTH && cursorY + row >= 0 && cursorY + row < FRAMEBUFFER_HEIGHT) {
                            framebuffer[(cursorY + row) * FRAMEBUFFER_WIDTH + (cursorX + col)] = colorIndex;
                        }
                    }
                }
            }
        }
        // Advance the cursor for the next character.
        cursorX += EmbeddedFont::FONT_WIDTH; // Character spacing is now built into the font glyphs.
    }
}

// === Sprite Rendering Implementation (Phase 5) ===

void AestheticLayer::DrawSprite(int spriteId, int x, int y, int w, int h, bool flipX, bool flipY) {
    if (!spriteSheet || !spriteSheet->IsLoaded()) {
        return; // No sprite sheet loaded
    }
    
    int tileSize = spriteSheet->GetTileSize();
    int spritesPerRow = SpriteSheet::SHEET_WIDTH / tileSize;
    
    // Draw w x h grid of sprites
    for (int ty = 0; ty < h; ty++) {
        for (int tx = 0; tx < w; tx++) {
            int currentSpriteId = spriteId + ty * spritesPerRow + tx;
            
            // Get sprite data
            std::vector<uint8_t> spriteData(tileSize * tileSize);
            if (!spriteSheet->GetSpriteData(currentSpriteId, spriteData.data())) {
                continue; // Sprite doesn't exist
            }
            
            // Calculate destination position
            int destX = x + (flipX ? (w - 1 - tx) : tx) * tileSize;
            int destY = y + (flipY ? (h - 1 - ty) : ty) * tileSize;
            
            // Draw sprite pixels
            for (int py = 0; py < tileSize; py++) {
                for (int px = 0; px < tileSize; px++) {
                    int srcX = flipX ? (tileSize - 1 - px) : px;
                    int srcY = flipY ? (tileSize - 1 - py) : py;
                    
                    uint8_t colorIndex = spriteData[srcY * tileSize + srcX];
                    
                    // Skip transparent pixels
                    if (transparentColorIndex >= 0 && colorIndex == static_cast<uint8_t>(transparentColorIndex)) {
                        continue;
                    }
                    
                    SetPixel(destX + px, destY + py, colorIndex);
                }
            }
        }
    }
}

void AestheticLayer::DrawSpriteSection(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    if (!spriteSheet || !spriteSheet->IsLoaded()) {
        return; // No sprite sheet loaded
    }
    
    // Simple version: no scaling (dw/dh ignored for now, assumes 1:1)
    // TODO: Add proper scaling support
    
    for (int py = 0; py < sh; py++) {
        for (int px = 0; px < sw; px++) {
            // Get pixel from sprite sheet
            uint8_t colorIndex = spriteSheet->GetPixel(sx + px, sy + py);
            
            // Skip transparent pixels
            if (transparentColorIndex >= 0 && colorIndex == static_cast<uint8_t>(transparentColorIndex)) {
                continue;
            }
            
            // Draw to destination (no scaling)
            int destX = dx + px;
            int destY = dy + py;
            
            SetPixel(destX, destY, colorIndex);
        }
    }
}

// === Map Rendering Implementation (Phase 5.8) ===

void AestheticLayer::DrawMap(Map* map, int mx, int my, int sx, int sy, int w, int h, uint8_t layerMask) {
    if (!map || !map->IsLoaded() || !spriteSheet || !spriteSheet->IsLoaded()) {
        return;
    }
    
    int tileSize = map->GetTileSize();
    int layerCount = map->GetLayerCount();
    
    // Render each layer
    for (int layer = 0; layer < layerCount; layer++) {
        // Check if this layer should be rendered
        if ((layerMask & (1 << layer)) == 0) {
            continue;
        }
        
        // Render tiles in this layer
        for (int ty = 0; ty < h; ty++) {
            for (int tx = 0; tx < w; tx++) {
                int mapX = mx + tx;
                int mapY = my + ty;
                
                uint8_t tileId = map->GetTile(mapX, mapY, layer);
                
                // 0 = empty/transparent
                if (tileId == 0) {
                    continue;
                }
                
                int screenX = sx + tx * tileSize;
                int screenY = sy + ty * tileSize;
                
                // Draw the sprite for this tile
                DrawSprite(tileId, screenX, screenY, 1, 1, false, false);
            }
        }
    }
}

void AestheticLayer::Present() {
    // Translate our color-indexed framebuffer to a 32-bit pixel buffer.
    for (size_t i = 0; i < framebuffer.size(); ++i) {
        uint8_t colorIndex = framebuffer[i];
        
        // Skip pixels marked as RGB (written by RectFillRGB)
        if (colorIndex == 255) {
            continue;  // Keep existing RGB value in pixelBuffer
        }
        
        const auto& color = palette[colorIndex];
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

void AestheticLayer::CaptureScreenshot() {
    // Convert pixelBuffer (ARGB8888) to RGBA format for PNG saving
    std::vector<uint8_t> rgba_data(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 4);
    
    for (size_t i = 0; i < pixelBuffer.size(); ++i) {
        uint32_t argb = pixelBuffer[i];
        
        // Extract ARGB components
        uint8_t a = (argb >> 24) & 0xFF;
        uint8_t r = (argb >> 16) & 0xFF;
        uint8_t g = (argb >> 8) & 0xFF;
        uint8_t b = argb & 0xFF;
        
        // Store as RGBA
        rgba_data[i * 4 + 0] = r;
        rgba_data[i * 4 + 1] = g;
        rgba_data[i * 4 + 2] = b;
        rgba_data[i * 4 + 3] = a;
    }
    
    // Save using Screenshot class
    Screenshot::SavePNG(rgba_data.data(), FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
}

const uint8_t* AestheticLayer::GetPixelData() const {
    // pixelBuffer is ARGB32, but for GIF we need to return the RGBA data
    // that was just created in CaptureScreenshot. Since we don't have it stored,
    // we return a pointer to a static buffer that gets updated each frame.
    // Actually, let's just return the raw pixelBuffer pointer - 
    // GifRecorder will handle conversion if needed.
    return reinterpret_cast<const uint8_t*>(pixelBuffer.data());
}