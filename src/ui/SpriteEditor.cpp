#include "ui/SpriteEditor.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "ui/UISystem.h"
#include "ui/SystemSprites.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // for memset
#include <algorithm> // for min/max

// stb_image for PNG loading/saving (implementation defined in SpriteSheet.cpp)
#include <stb_image.h>
#include <stb_image_write.h>

SpriteEditor::SpriteEditor()
    : currentSpriteIndex(0)
    , currentTool(Tool::PENCIL)
    , selectedColor(7)  // Default to white
    , isActive(false)
    , zoom(16)  // PICO-8 style: 16x zoom = 128x128 display
    , isDragging(false)
    , dragStartX(0)
    , dragStartY(0)
    , systemSprites(nullptr)
    , historyIndex(-1)  // No history initially
{
    // Open log file
    logFile.open("sprite_editor_log.txt", std::ios::out | std::ios::trunc);
    if (logFile.is_open()) {
        Log("=== SPRITE EDITOR LOG ===");
        Log("SpriteEditor initialized");
    }
    
    // Initialize canvas to transparent (color 0)
    ClearCanvas();
    
    // Initialize all sprites in spritesheet to transparent
    std::memset(spriteSheet, 0, sizeof(spriteSheet));
}

SpriteEditor::~SpriteEditor() {
    Log("SpriteEditor destroyed");
    if (logFile.is_open()) {
        logFile.close();
    }
}

void SpriteEditor::Initialize(const std::string& path) {
    spritesheetPath = path;
    LoadSpritesheet();
}

void SpriteEditor::Update(InputManager& input) {
    static int updateCount = 0;
    updateCount++;
    
    if (!isActive) {
        if (updateCount % 60 == 0) {
            Log("[Update] Not active, skipping update. Count: " + std::to_string(updateCount));
        }
        return;
    }
    
    // Log every 60 frames (once per second)
    if (updateCount % 60 == 0) {
        Log("[Update] Active! Update count: " + std::to_string(updateCount));
    }
    
    // Handle keyboard shortcuts
    HandleKeyboard(input);
    
    // Handle mouse input
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    bool leftButtonDown = input.isMouseButtonDown(1);  // SDL uses 1 for left button!
    bool leftButtonPressed = input.isMouseButtonPressed(1);  // Just pressed this frame
    
    // Check if in canvas
    bool inCanvas = (mouseX >= CANVAS_X && mouseX < CANVAS_X + CANVAS_SIZE &&
                     mouseY >= CANVAS_Y && mouseY < CANVAS_Y + CANVAS_SIZE);
    
    // Handle drag for line/rect tools
    if (inCanvas) {
        int canvasX = ScreenToCanvasX(mouseX);
        int canvasY = ScreenToCanvasY(mouseY);
        
        // Start drag on press
        if (leftButtonPressed && (currentTool == Tool::LINE || currentTool == Tool::RECT)) {
            isDragging = true;
            dragStartX = canvasX;
            dragStartY = canvasY;
        }
        
        // End drag on release
        if (!leftButtonDown && isDragging) {
            if (currentTool == Tool::LINE) {
                UseLine(dragStartX, dragStartY, canvasX, canvasY);
            } else if (currentTool == Tool::RECT) {
                UseRect(dragStartX, dragStartY, canvasX, canvasY, false);  // outline for now
            }
            isDragging = false;
        }
        
        // Handle single-click tools
        if (leftButtonDown && !isDragging) {
            HandleCanvasClick(mouseX, mouseY);
        }
    } else {
        // Outside canvas - cancel drag
        isDragging = false;
        
        // Handle other UI areas
        if (leftButtonPressed) {
            // Check palette area (4x4 grid)
            if (mouseX >= PALETTE_X && mouseX < PALETTE_X + (PALETTE_COLS * COLOR_BOX_SIZE) &&
                     mouseY >= PALETTE_Y && mouseY < PALETTE_Y + (PALETTE_ROWS * COLOR_BOX_SIZE)) {
                HandlePaletteClick(mouseX, mouseY);
            }
            // Check spritesheet area
            else if (mouseX >= SHEET_X && mouseX < SHEET_X + (SHEET_COLS * SHEET_SPRITE_SIZE) &&
                     mouseY >= SHEET_Y && mouseY < SHEET_Y + (SHEET_ROWS * SHEET_SPRITE_SIZE)) {
                int col = (mouseX - SHEET_X) / SHEET_SPRITE_SIZE;
                int row = (mouseY - SHEET_Y) / SHEET_SPRITE_SIZE;
                int newIndex = row * SHEET_COLS + col;
                if (newIndex >= 0 && newIndex < 256) {
                    SwitchSprite(newIndex);
                }
            }
            // Check toolbar area
            else if (mouseY >= TOOLBAR_Y) {
                HandleToolbarClick(mouseX, mouseY);
            }
        }
    }
}

void SpriteEditor::Render(AestheticLayer& renderer) {
    if (!isActive) return;
    
    // Clear screen
    renderer.Clear(UISystem::COLOR_DARK_BLUE);
    
    // Render all components (PICO-8 style layout)
    RenderCanvas(renderer);        // Large sprite (left, 128x128)
    RenderPalette(renderer);       // Palette (right, vertical)
    RenderSpritesheet(renderer);   // Spritesheet grid (bottom)
    RenderToolbar(renderer);       // Tools (bottom right)
}

// ===== Rendering Methods =====

void SpriteEditor::RenderCanvas(AestheticLayer& renderer) {
    // Draw canvas background
    renderer.RectFill(CANVAS_X, CANVAS_Y, CANVAS_SIZE, CANVAS_SIZE, UISystem::COLOR_BACKGROUND);
    
    // Draw pixels with 16x zoom
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            uint8_t color = canvas[y][x];
            if (color != 0) {  // Skip transparent pixels
                int screenX = CANVAS_X + (x * CANVAS_ZOOM);
                int screenY = CANVAS_Y + (y * CANVAS_ZOOM);
                renderer.RectFill(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM, color);
            }
        }
    }
    
    // Draw grid
    for (int i = 0; i <= 8; i++) {
        // Vertical lines
        int x = CANVAS_X + (i * CANVAS_ZOOM);
        renderer.Line(x, CANVAS_Y, x, CANVAS_Y + CANVAS_SIZE - 1, UISystem::COLOR_DARK_GRAY);
        
        // Horizontal lines
        int y = CANVAS_Y + (i * CANVAS_ZOOM);
        renderer.Line(CANVAS_X, y, CANVAS_X + CANVAS_SIZE - 1, y, UISystem::COLOR_DARK_GRAY);
    }
    
    // Canvas border
    renderer.Rect(CANVAS_X - 1, CANVAS_Y - 1, CANVAS_SIZE + 2, CANVAS_SIZE + 2, UISystem::COLOR_WHITE);
}

void SpriteEditor::RenderPalette(AestheticLayer& renderer) {
    for (int row = 0; row < PALETTE_ROWS; row++) {
        for (int col = 0; col < PALETTE_COLS; col++) {
            int colorIndex = row * PALETTE_COLS + col;
            int x = PALETTE_X + (col * COLOR_BOX_SIZE);
            int y = PALETTE_Y + (row * COLOR_BOX_SIZE);
            
            // Draw color box
            renderer.RectFill(x, y, COLOR_BOX_SIZE, COLOR_BOX_SIZE, colorIndex);
            
            // Highlight selected color with white border
            if (colorIndex == selectedColor) {
                renderer.Rect(x - 1, y - 1, COLOR_BOX_SIZE + 2, COLOR_BOX_SIZE + 2, UISystem::COLOR_WHITE);
            }
        }
    }
}

void SpriteEditor::RenderSpritesheet(AestheticLayer& renderer) {
    // Render 16x8 grid of sprites (128 sprites visible)
    for (int row = 0; row < SHEET_ROWS; row++) {
        for (int col = 0; col < SHEET_COLS; col++) {
            int spriteIndex = row * SHEET_COLS + col;
            int sheetX = SHEET_X + (col * SHEET_SPRITE_SIZE);
            int sheetY = SHEET_Y + (row * SHEET_SPRITE_SIZE);
            
            // Draw each sprite at 8x8 size (1:1, no zoom)
            for (int py = 0; py < 8; py++) {
                for (int px = 0; px < 8; px++) {
                    uint8_t color = spriteSheet[spriteIndex][py][px];
                    if (color != 0) {  // Skip transparent
                        renderer.RectFill(sheetX + px, sheetY + py, 1, 1, color);
                    }
                }
            }
            
            // Highlight currently selected sprite
            if (spriteIndex == currentSpriteIndex) {
                renderer.Rect(sheetX - 1, sheetY - 1, 
                             SHEET_SPRITE_SIZE + 2, SHEET_SPRITE_SIZE + 2, 
                             UISystem::COLOR_YELLOW);
            }
        }
    }
    
    // Draw grid lines
    renderer.Rect(SHEET_X - 1, SHEET_Y - 1, 
                 SHEET_COLS * SHEET_SPRITE_SIZE + 2, 
                 SHEET_ROWS * SHEET_SPRITE_SIZE + 2, 
                 UISystem::COLOR_DARK_GRAY);
}

void SpriteEditor::RenderToolbar(AestheticLayer& renderer) {
    const int iconIds[] = { 0, 1, 2, 3, 4 };  // Icon indices from SystemSprites::Icon
    const int BUTTON_SIZE = 16;  // Square buttons 16x16
    const int BUTTON_SPACING = 4;
    
    for (int i = 0; i < 5; i++) {
        int x = CANVAS_X + (i * (BUTTON_SIZE + BUTTON_SPACING));
        int y = TOOLBAR_Y;
        
        // Square background (gray, yellow if selected)
        uint8_t bgColor = (static_cast<int>(currentTool) == i) 
            ? UISystem::COLOR_YELLOW 
            : UISystem::COLOR_DARK_GRAY;
        renderer.RectFill(x, y, BUTTON_SIZE, BUTTON_SIZE, bgColor);
        
        // Draw icon centered in button
        if (systemSprites) {
            systemSprites->DrawSprite(renderer, iconIds[i], x + 4, y + 4, 1);
        } else {
            // Fallback to text if systemSprites not set
            const char* tools[] = { "P", "F", "L", "R", "C" };
            uint8_t textColor = (static_cast<int>(currentTool) == i)
                ? UISystem::COLOR_BACKGROUND
                : UISystem::COLOR_WHITE;
            renderer.Print(tools[i], x + 4, y + 4, textColor);
        }
    }
}

// ===== Input Handling =====

void SpriteEditor::HandleCanvasClick(int mouseX, int mouseY) {
    int canvasX = ScreenToCanvasX(mouseX);
    int canvasY = ScreenToCanvasY(mouseY);
    
    Log("[HandleCanvasClick] Screen(" + std::to_string(mouseX) + "," + std::to_string(mouseY) + 
        ") -> Canvas(" + std::to_string(canvasX) + "," + std::to_string(canvasY) + ")");
    
    if (!IsInCanvas(canvasX, canvasY)) {
        Log("[HandleCanvasClick] Out of bounds!");
        return;
    }
    
    Log("[HandleCanvasClick] Current tool: " + std::to_string(static_cast<int>(currentTool)) + 
        ", Selected color: " + std::to_string(selectedColor));
    
    // Save history before any modification
    SaveHistoryState();
    
    switch (currentTool) {
        case Tool::PENCIL:
            Log("[HandleCanvasClick] Using PENCIL");
            UsePencil(canvasX, canvasY);
            break;
        
        case Tool::FILL:
            Log("[HandleCanvasClick] Using FILL");
            UseFill(canvasX, canvasY, canvas[canvasY][canvasX], selectedColor);
            break;
        
        case Tool::PICKER:
            Log("[HandleCanvasClick] Using PICKER");
            UsePicker(canvasX, canvasY);
            break;
            
        default:
            break;
    }
}

void SpriteEditor::HandlePaletteClick(int mouseX, int mouseY) {
    int col = (mouseX - PALETTE_X) / COLOR_BOX_SIZE;
    int row = (mouseY - PALETTE_Y) / COLOR_BOX_SIZE;
    
    if (col >= 0 &&col < PALETTE_COLS && row >= 0 && row < PALETTE_ROWS) {
        selectedColor = row * PALETTE_COLS + col;
        Log("[HandlePaletteClick] Selected color: " + std::to_string(selectedColor));
    }
}

void SpriteEditor::HandleToolbarClick(int mouseX, int mouseY) {
    const int BUTTON_SIZE = 16;
    const int BUTTON_SPACING = 4;
    
    // Check if within toolbar Y range
    if (mouseY < TOOLBAR_Y || mouseY > TOOLBAR_Y + BUTTON_SIZE) return;
    
    // Check each button
    for (int i = 0; i < 5; i++) {
        int buttonX = CANVAS_X + (i * (BUTTON_SIZE + BUTTON_SPACING));
        if (mouseX >= buttonX && mouseX < buttonX + BUTTON_SIZE) {
            currentTool = static_cast<Tool>(i);
            return;
        }
    }
}

void SpriteEditor::HandleKeyboard(InputManager& input) {
    // Tool shortcuts
    if (input.isKeyPressed(SDL_SCANCODE_P)) currentTool = Tool::PENCIL;
    if (input.isKeyPressed(SDL_SCANCODE_F)) currentTool = Tool::FILL;
    if (input.isKeyPressed(SDL_SCANCODE_L)) currentTool = Tool::LINE;
    if (input.isKeyPressed(SDL_SCANCODE_R)) currentTool = Tool::RECT;
    if (input.isKeyPressed(SDL_SCANCODE_C)) currentTool = Tool::PICKER;
    
    // Check for Ctrl key
    bool ctrlHeld = input.isKeyDown(SDL_SCANCODE_LCTRL) || input.isKeyDown(SDL_SCANCODE_RCTRL);
    
    if (ctrlHeld) {
        // Undo (Ctrl+Z)
        if (input.isKeyPressed(SDL_SCANCODE_Z)) {
            Undo();
        }
        // Redo (Ctrl+Y)
        if (input.isKeyPressed(SDL_SCANCODE_Y)) {
            Redo();
        }
        // Save spritesheet (Ctrl+S)
        if (input.isKeyPressed(SDL_SCANCODE_S)) {
            SaveSpritesheet();
        }
    }
    
    // Sprite navigation
    // Left/Right: Navigate by 1 sprite
    if (input.isKeyPressed(SDL_SCANCODE_LEFT) && currentSpriteIndex > 0) {
        SwitchSprite(currentSpriteIndex - 1);
    }
    if (input.isKeyPressed(SDL_SCANCODE_RIGHT) && currentSpriteIndex < 255) {
        SwitchSprite(currentSpriteIndex + 1);
    }
    
    // Up/Down: Navigate by row (16 sprites)
    if (input.isKeyPressed(SDL_SCANCODE_UP) && currentSpriteIndex >= 16) {
        SwitchSprite(currentSpriteIndex - 16);
    }
    if (input.isKeyPressed(SDL_SCANCODE_DOWN) && currentSpriteIndex < 240) {
        SwitchSprite(currentSpriteIndex + 16);
    }
}

// ===== Drawing Tools =====

void SpriteEditor::UsePencil(int x, int y) {
    canvas[y][x] = selectedColor;
}

void SpriteEditor::UseFill(int x, int y, uint8_t targetColor, uint8_t replacementColor) {
    // Prevent infinite recursion
    if (targetColor == replacementColor) return;
    if (!IsInCanvas(x, y)) return;
    if (canvas[y][x] != targetColor) return;
    
    // Flood fill
    canvas[y][x] = replacementColor;
    
    UseFill(x + 1, y, targetColor, replacementColor);
    UseFill(x - 1, y, targetColor, replacementColor);
    UseFill(x, y + 1, targetColor, replacementColor);
    UseFill(x, y - 1, targetColor, replacementColor);
}

void SpriteEditor::UseLine(int x1, int y1, int x2, int y2) {
    // Bresenham's line algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1;
    int y = y1;
    
    while (true) {
        // Draw pixel if in bounds
        if (IsInCanvas(x, y)) {
            canvas[y][x] = selectedColor;
        }
        
        // Check if we reached the end
        if (x == x2 && y == y2) break;
        
        // Calculate error and step
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void SpriteEditor::UseRect(int x1, int y1, int x2, int y2, bool filled) {
    // Normalize coordinates
    int minX = std::min(x1, x2);
    int maxX = std::max(x1, x2);
    int minY = std::min(y1, y2);
    int maxY = std::max(y1, y2);
    
    if (filled) {
        // Filled rectangle
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                if (IsInCanvas(x, y)) {
                    canvas[y][x] = selectedColor;
                }
            }
        }
    } else {
        // Rectangle outline
        // Top and bottom edges
        for (int x = minX; x <= maxX; x++) {
            if (IsInCanvas(x, minY)) canvas[minY][x] = selectedColor;
            if (IsInCanvas(x, maxY)) canvas[maxY][x] = selectedColor;
        }
        // Left and right edges
        for (int y = minY; y <= maxY; y++) {
            if (IsInCanvas(minX, y)) canvas[y][minX] = selectedColor;
            if (IsInCanvas(maxX, y)) canvas[y][maxX] = selectedColor;
        }
    }
}

void SpriteEditor::UsePicker(int x, int y) {
    selectedColor = canvas[y][x];
}

// ===== Helper Methods =====

void SpriteEditor::ClearCanvas() {
    std::memset(canvas, 0, sizeof(canvas));
}

void SpriteEditor::SwitchSprite(int newIndex) {
    // Save current sprite to spritesheet
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
    
    // Load new sprite
    currentSpriteIndex = newIndex;
    std::memcpy(canvas, spriteSheet[currentSpriteIndex], sizeof(canvas));
    
    std::cout << "Switched to sprite #" << currentSpriteIndex << std::endl;
}

int SpriteEditor::ScreenToCanvasX(int screenX) {
    return (screenX - CANVAS_X) / CANVAS_ZOOM;
}

int SpriteEditor::ScreenToCanvasY(int screenY) {
    return (screenY - CANVAS_Y) / CANVAS_ZOOM;
}

bool SpriteEditor::IsInCanvas(int canvasX, int canvasY) {
    return canvasX >= 0 && canvasX < 8 && canvasY >= 0 && canvasY < 8;
}

void SpriteEditor::SaveSpritesheet() {
    if (spritesheetPath.empty()) {
        Log("[SaveSpritesheet] No path set, cannot save");
        return;
    }
    
    // Save current sprite to spritesheet array first
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
    
    // Spritesheet is 128x128 (16x16 grid of 8x8 sprites)
    const int SHEET_WIDTH = 128;
    const int SHEET_HEIGHT = 128;
    const int SPRITES_PER_ROW = 16;
    
    // Create RGBA buffer (4 channels)
    std::vector<uint8_t> imageData(SHEET_WIDTH * SHEET_HEIGHT * 4);
    
    // Convert sprites to RGBA image
    for (int spriteIndex = 0; spriteIndex < 256; spriteIndex++) {
        int gridX = spriteIndex % SPRITES_PER_ROW;
        int gridY = spriteIndex / SPRITES_PER_ROW;
        
        for (int py = 0; py < 8; py++) {
            for (int px = 0; px < 8; px++) {
                uint8_t paletteIndex = spriteSheet[spriteIndex][py][px];
                
                // Calculate position in 128x128 image
                int imageX = gridX * 8 + px;
                int imageY = gridY * 8 + py;
                int pixelIndex = (imageY * SHEET_WIDTH + imageX) * 4;
                
                // Store palette index as grayscale (16 levels)
                uint8_t value = paletteIndex * 16;
                imageData[pixelIndex + 0] = value;  // R
                imageData[pixelIndex + 1] = value;  // G
                imageData[pixelIndex + 2] = value;  // B
                imageData[pixelIndex + 3] = 255;    // A (opaque)
            }
        }
    }
    
    // Write PNG
    if (stbi_write_png(spritesheetPath.c_str(), SHEET_WIDTH, SHEET_HEIGHT, 4, 
                       imageData.data(), SHEET_WIDTH * 4)) {
        Log("[SaveSpritesheet] Saved successfully to: " + spritesheetPath);
    } else {
        Log("[SaveSpritesheet] ERROR: Failed to save to: " + spritesheetPath);
    }
}

void SpriteEditor::LoadSpritesheet() {
    if (spritesheetPath.empty()) {
        Log("[LoadSpritesheet] No path set");
        return;
    }
    
    int width, height, channels;
    unsigned char* data = stbi_load(spritesheetPath.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        Log("[LoadSpritesheet] Could not load: " + spritesheetPath + " (file may not exist yet)");
        return;
    }
    
    if (width != 128 || height != 128) {
        Log("[LoadSpritesheet] WARNING: Expected 128x128, got " + 
            std::to_string(width) + "x" + std::to_string(height));
        stbi_image_free(data);
        return;
    }
    
    // Parse sprites from 128x128 image (16x16 grid of 8x8 sprites)
    const int SPRITES_PER_ROW = 16;
    
    for (int spriteIndex = 0; spriteIndex < 256; spriteIndex++) {
        int gridX = spriteIndex % SPRITES_PER_ROW;
        int gridY = spriteIndex / SPRITES_PER_ROW;
        
        for (int py = 0; py < 8; py++) {
            for (int px = 0; px < 8; px++) {
                int imageX = gridX * 8 + px;
                int imageY = gridY * 8 + py;
                int pixelIndex = (imageY * 128 + imageX) * 4;
                
                // Convert grayscale back to palette index (0-15)
                uint8_t value = data[pixelIndex];  // R channel
                uint8_t paletteIndex = value / 16;
                if (paletteIndex > 15) paletteIndex = 15;
                
                spriteSheet[spriteIndex][py][px] = paletteIndex;
            }
        }
    }
    
    // Load first sprite into canvas
    std::memcpy(canvas, spriteSheet[0], sizeof(canvas));
    
    stbi_image_free(data);
    Log("[LoadSpritesheet] Loaded successfully from: " + spritesheetPath);
}


void SpriteEditor::Log(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush();  // Ensure it writes immediately
    }
}

// ===== Undo/Redo System =====

void SpriteEditor::SaveHistoryState() {
    // Remove any redo history (everything after current index)
    if (historyIndex >= 0 && historyIndex < (int)undoHistory.size() - 1) {
        undoHistory.erase(undoHistory.begin() + historyIndex + 1, undoHistory.end());
    }
    
    // Save current canvas state
    CanvasState state;
    std::memcpy(state.data, canvas, sizeof(canvas));
    undoHistory.push_back(state);
    
    // Limit history size
    if (undoHistory.size() > MAX_HISTORY) {
        undoHistory.erase(undoHistory.begin());
    } else {
        historyIndex++;
    }
}

void SpriteEditor::Undo() {
    if (historyIndex <= 0) return;  // No history to undo
    
    historyIndex--;
    std::memcpy(canvas, undoHistory[historyIndex].data, sizeof(canvas));
    
    // Update spritesheet
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
}

void SpriteEditor::Redo() {
    if (historyIndex >= (int)undoHistory.size() - 1) return;  // No history to redo
    
    historyIndex++;
    std::memcpy(canvas, undoHistory[historyIndex].data, sizeof(canvas));
    
    // Update spritesheet
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
}
