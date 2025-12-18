#include "ui/SpriteEditor.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "ui/UISystem.h"
#include "ui/SystemSprites.h"
#include "utils/FileDialog.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // for memset
#include <algorithm> // for min/max
#include <ctime>  // for timestamp
#include <filesystem>  // for directory creation

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
    
    // Load recent files list
    LoadRecentFiles();
}

SpriteEditor::~SpriteEditor() {
    SaveRecentFiles();  // Save before closing
    
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
    const int iconIds[] = { 0, 1, 2, 3, 4 };  // Tool icon indices
    const int BUTTON_SIZE = 16;
    const int BUTTON_SPACING = 4;
    
    // Draw tool buttons (Pencil, Fill, Line, Rect, Picker)
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
            const char* tools[] = { "P", "F", "L", "R", "C" };
            uint8_t textColor = (static_cast<int>(currentTool) == i)
                ? UISystem::COLOR_BACKGROUND
                : UISystem::COLOR_WHITE;
            renderer.Print(tools[i], x + 4, y + 4, textColor);
        }
    }
    
    // Separator (small gap)
    int separatorX = CANVAS_X + (5 * (BUTTON_SIZE + BUTTON_SPACING)) + 4;
    
    // Import button (SystemSprites ID 6 - LOAD icon)
    int importX = separatorX;
    renderer.RectFill(importX, TOOLBAR_Y, BUTTON_SIZE, BUTTON_SIZE, UISystem::COLOR_DARK_GRAY);
    if (systemSprites) {
        systemSprites->DrawSprite(renderer, 6, importX + 4, TOOLBAR_Y + 4, 1);  // LOAD icon
    } else {
        renderer.Print("I", importX + 4, TOOLBAR_Y + 4, UISystem::COLOR_WHITE);
    }
    
    // Export button (SystemSprites ID 5 - SAVE icon)
    int exportX = importX + BUTTON_SIZE + BUTTON_SPACING;
    renderer.RectFill(exportX, TOOLBAR_Y, BUTTON_SIZE, BUTTON_SIZE, UISystem::COLOR_DARK_GRAY);
    if (systemSprites) {
        systemSprites->DrawSprite(renderer, 5, exportX + 4, TOOLBAR_Y + 4, 1);  // SAVE icon
    } else {
        renderer.Print("E", exportX + 4, TOOLBAR_Y + 4, UISystem::COLOR_WHITE);
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
    
    // Only save history if we're actually going to change something
    bool willModify = false;
    
    switch (currentTool) {
        case Tool::PENCIL:
            // Check if pixel will actually change
            if (canvas[canvasY][canvasX] != selectedColor) {
                willModify = true;
            }
            break;
        case Tool::FILL:
            // Fill always modifies (unless target == replacement)
            if (canvas[canvasY][canvasX] != selectedColor) {
                willModify = true;
            }
            break;
        case Tool::PICKER:
            // Picker never modifies
            willModify = false;
            break;
    }
    
    // Only save state if something will change
    if (willModify) {
        SaveHistoryState();
    }
    
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
    
    // Check tool buttons (0-4)
    for (int i = 0; i < 5; i++) {
        int buttonX = CANVAS_X + (i * (BUTTON_SIZE + BUTTON_SPACING));
        if (mouseX >= buttonX && mouseX < buttonX + BUTTON_SIZE) {
            currentTool = static_cast<Tool>(i);
            return;
        }
    }
    
    // Check Import/Export buttons
    int separatorX = CANVAS_X + (5 * (BUTTON_SIZE + BUTTON_SPACING)) + 4;
    
    // Import button
    int importX = separatorX;
    if (mouseX >= importX && mouseX < importX + BUTTON_SIZE) {
        Log("[HandleToolbarClick] Import button clicked");
        OnImportClicked();
        return;
    }
    
    // Export button
    int exportX = importX + BUTTON_SIZE + BUTTON_SPACING;
    if (mouseX >= exportX && mouseX < exportX + BUTTON_SIZE) {
        Log("[HandleToolbarClick] Export button clicked");
        OnExportClicked();
        return;
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
    bool shiftHeld = input.isKeyDown(SDL_SCANCODE_LSHIFT) || input.isKeyDown(SDL_SCANCODE_RSHIFT);
    
    if (ctrlHeld) {
        // Undo (Ctrl+Z) or Redo (Ctrl+Shift+Z)
        if (input.isKeyPressed(SDL_SCANCODE_Z)) {
            if (shiftHeld) {
                Log("[REDO TRIGGERED]");
                Redo();
            } else {
                Log("[UNDO TRIGGERED]");
                Undo();
            }
        }
        // Redo alternative (Ctrl+Y)
        if (input.isKeyPressed(SDL_SCANCODE_Y)) {
            Log("[REDO TRIGGERED (Ctrl+Y)]");
            Redo();
        }
        // Save spritesheet (Ctrl+S)
        if (input.isKeyPressed(SDL_SCANCODE_S)) {
            SaveSpritesheet();
            Log("[Save] Spritesheet saved");
        }
        // Import spritesheet (Ctrl+O)
        if (input.isKeyPressed(SDL_SCANCODE_O)) {
            Log("[Hotkey] Ctrl+O - Import triggered");
            OnImportClicked();
        }
        // Export spritesheet (Ctrl+E) or current sprite (Ctrl+Shift+E)
        if (input.isKeyPressed(SDL_SCANCODE_E)) {
            if (shiftHeld) {
                Log("[Hotkey] Ctrl+Shift+E - Export current sprite triggered");
                OnExportCurrentSprite();
            } else {
                Log("[Hotkey] Ctrl+E - Export triggered");
                OnExportClicked();
            }
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
    
    // Clear undo/redo history for this sprite
    undoStack.clear();
    redoStack.clear();
    
    Log("Switched to sprite #" + std::to_string(currentSpriteIndex) + " - History cleared");
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
    // Save current canvas state to undo stack BEFORE modification
    CanvasState state;
    std::memcpy(state.data, canvas, sizeof(canvas));
    undoStack.push_back(state);
    
    // Any new modification clears the redo stack
    redoStack.clear();
    
    // Limit undo stack size
    if (undoStack.size() > MAX_UNDO_LEVELS) {
        undoStack.erase(undoStack.begin());
    }
    
    Log("[SaveHistory] Saved to undo stack. Stack size: " + std::to_string(undoStack.size()));
}

void SpriteEditor::Undo() {
    if (undoStack.empty()) {
        Log("[Undo] Nothing to undo");
        return;
    }
    
    // Save current state to redo stack
    CanvasState currentState;
    std::memcpy(currentState.data, canvas, sizeof(canvas));
    redoStack.push_back(currentState);
    
    // Restore last state from undo stack
    CanvasState previousState = undoStack.back();
    undoStack.pop_back();
    
    std::memcpy(canvas, previousState.data, sizeof(canvas));
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
    
    Log("[Undo] Restored. Undo stack: " + std::to_string(undoStack.size()) + 
        ", Redo stack: " + std::to_string(redoStack.size()));
}

void SpriteEditor::Redo() {
    if (redoStack.empty()) {
        Log("[Redo] Nothing to redo");
        return;
    }
    
    // Save current state to undo stack
    CanvasState currentState;
    std::memcpy(currentState.data, canvas, sizeof(canvas));
    undoStack.push_back(currentState);
    
    // Restore last state from redo stack
    CanvasState nextState = redoStack.back();
    redoStack.pop_back();
    
    std::memcpy(canvas, nextState.data, sizeof(canvas));
    std::memcpy(spriteSheet[currentSpriteIndex], canvas, sizeof(canvas));
    
    Log("[Redo] Restored. Undo stack: " + std::to_string(undoStack.size()) + 
        ", Redo stack: " + std::to_string(redoStack.size()));
}

// ===== Import/Export System =====

// Validate PNG file (check if it's 128x128)
bool SpriteEditor::ValidatePNG(const std::string& filepath, int& width, int& height) {
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, nullptr, 4);
    
    if (!data) {
        Log("[ValidatePNG] Failed to load: " + filepath);
        return false;
    }
    
    stbi_image_free(data);
    
    if (width != 128 || height != 128) {
        Log("[ValidatePNG] Invalid size: " + std::to_string(width) + "x" + std::to_string(height) + 
            " (expected 128x128)");
        return false;
    }
    
    Log("[ValidatePNG] Valid PNG: 128x128");
    return true;
}

// Import spritesheet from external PNG
bool SpriteEditor::ImportSpritesheet(const std::string& filepath) {
    Log("[ImportSpritesheet] Loading from: " + filepath);
    
    // Validate first
    int width, height;
    if (!ValidatePNG(filepath, width, height)) {
        return false;
    }
    
    // Load PNG data
    int channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        Log("[ImportSpritesheet] Failed to load image data");
        return false;
    }
    
    // Parse 128x128 image into 256 sprites (16x16 grid)
    const int SPRITES_PER_ROW = 16;
    const int SPRITE_SIZE = 8;
    
    for (int spriteIndex = 0; spriteIndex < 256; spriteIndex++) {
        int gridX = spriteIndex % SPRITES_PER_ROW;
        int gridY = spriteIndex / SPRITES_PER_ROW;
        
        for (int py = 0; py < SPRITE_SIZE; py++) {
            for (int px = 0; px < SPRITE_SIZE; px++) {
                int imageX = gridX * SPRITE_SIZE + px;
                int imageY = gridY * SPRITE_SIZE + py;
                int pixelIndex = (imageY * 128 + imageX) * 4;
                
                // Convert grayscale to palette index (0-15)
                uint8_t grayValue = data[pixelIndex];  // R channel
                uint8_t paletteIndex = grayValue / 16;  // 0-255 -> 0-15
                
                spriteSheet[spriteIndex][py][px] = paletteIndex;
            }
        }
    }
    
    // Reload current sprite to canvas
    std::memcpy(canvas, spriteSheet[currentSpriteIndex], sizeof(canvas));
    
    // Clear undo/redo since we loaded new data
    undoStack.clear();
    redoStack.clear();
    
    stbi_image_free(data);
    Log("[ImportSpritesheet] Successfully imported 256 sprites");
    return true;
}

// Export spritesheet to external PNG
bool SpriteEditor::ExportSpritesheet(const std::string& filepath) {
    Log("[ExportSpritesheet] Exporting to: " + filepath);
    
    // Use existing SaveSpritesheet logic but to custom path
    const int SHEET_WIDTH = 128;
    const int SPRITES_PER_ROW = 16;
    std::vector<uint8_t> imageData(SHEET_WIDTH * SHEET_WIDTH * 4);
    
    for (int spriteIndex = 0; spriteIndex < 256; spriteIndex++) {
        int gridX = spriteIndex % SPRITES_PER_ROW;
        int gridY = spriteIndex / SPRITES_PER_ROW;
        
        for (int py = 0; py < 8; py++) {
            for (int px = 0; px < 8; px++) {
                uint8_t paletteIndex = spriteSheet[spriteIndex][py][px];
                
                int imageX = gridX * 8 + px;
                int imageY = gridY * 8 + py;
                int pixelIndex = (imageY * SHEET_WIDTH + imageX) * 4;
                
                uint8_t value = paletteIndex * 16;
                imageData[pixelIndex + 0] = value;  // R
                imageData[pixelIndex + 1] = value;  // G
                imageData[pixelIndex + 2] = value;  // B
                imageData[pixelIndex + 3] = 255;    // A
            }
        }
    }
    
    int result = stbi_write_png(filepath.c_str(), SHEET_WIDTH, SHEET_WIDTH, 4, 
                                 imageData.data(), SHEET_WIDTH * 4);
    
    if (result == 0) {
        Log("[ExportSpritesheet] Failed to write PNG");
        return false;
    }
    
    Log("[ExportSpritesheet] Successfully exported to: " + filepath);
    return true;
}

// Export single sprite (8×8 or scaled)
bool SpriteEditor::ExportSingleSprite(int spriteIndex, const std::string& filepath, int scale) {
    if (spriteIndex < 0 || spriteIndex >= 256) {
        Log("[ExportSingleSprite] Invalid sprite index: " + std::to_string(spriteIndex));
        return false;
    }
    
    Log("[ExportSingleSprite] Exporting sprite #" + std::to_string(spriteIndex) + 
        " at " + std::to_string(scale) + "x scale");
    
    int size = 8 * scale;
    std::vector<uint8_t> imageData(size * size * 4);
    
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            uint8_t paletteIndex = spriteSheet[spriteIndex][py][px];
            uint8_t value = paletteIndex * 16;
            
            // Draw scaled pixel (scale×scale block)
            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    int imageX = px * scale + sx;
                    int imageY = py * scale + sy;
                    int pixelIndex = (imageY * size + imageX) * 4;
                    
                    imageData[pixelIndex + 0] = value;  // R
                    imageData[pixelIndex + 1] = value;  // G
                    imageData[pixelIndex + 2] = value;  // B
                    imageData[pixelIndex + 3] = 255;    // A
                }
            }
        }
    }
    
    int result = stbi_write_png(filepath.c_str(), size, size, 4, 
                                 imageData.data(), size * 4);
    
    if (result == 0) {
        Log("[ExportSingleSprite] Failed to write PNG");
        return false;
    }
    
    Log("[ExportSingleSprite] Successfully exported: " + filepath);
    return true;
}

void SpriteEditor::OnImportClicked() {
    Log("[Import] Button clicked - Opening file dialog...");
    
    std::string filepath = FileDialog::OpenFile(
        "PNG Files\0*.png\0All Files\0*.*\0",
        "Import Spritesheet"
    );
    
    if (filepath.empty()) {
        Log("[Import] User cancelled");
        return;
    }
    
    Log("[Import] Selected file: " + filepath);
    
    if (ImportSpritesheet(filepath)) {
        AddToRecentFiles(filepath);  // Add to recent list
        Log("[Import] Import successful!");
        std::cout << "Successfully imported: " << filepath << std::endl;
    } else {
        Log("[Import] Import failed - check that PNG is 128x128");
        std::cout << "Import FAILED. PNG must be 128x128 pixels." << std::endl;
    }
}

void SpriteEditor::OnExportClicked() {
    Log("[Export] Button clicked - Opening save dialog...");
    
    // Generate default filename with timestamp
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char defaultName[256];
    sprintf_s(defaultName, "spritesheet_%04d-%02d-%02d.png",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    
    std::string filepath = FileDialog::SaveFile(
        defaultName,
        "PNG Files\0*.png\0All Files\0*.*\0",
        "Export Spritesheet",
        "png"
    );
    
    if (filepath.empty()) {
        Log("[Export] User cancelled");
        return;
    }
    
    Log("[Export] Selected file: " + filepath);
    
    if (ExportSpritesheet(filepath)) {
        Log("[Export] Export successful!");
        std::cout << "Successfully exported to: " << filepath << std::endl;
    } else {
        Log("[Export] Export failed");
        std::cout << "Export FAILED." << std::endl;
    }
}

void SpriteEditor::OnExportCurrentSprite() {
    Log("[ExportCurrent] Export current sprite triggered");
    
    // Generate default filename
    char defaultName[256];
    sprintf_s(defaultName, "sprite_%03d.png", currentSpriteIndex);
    
    std::string filepath = FileDialog::SaveFile(
        defaultName,
        "PNG Files\0*.png\0All Files\0*.*\0",
        "Export Current Sprite",
        "png"
    );
    
    if (filepath.empty()) {
        Log("[ExportCurrent] User cancelled");
        return;
    }
    
    Log("[ExportCurrent] Selected file: " + filepath);
    
    // Export at 16× scale (128×128) for better visibility
    if (ExportSingleSprite(currentSpriteIndex, filepath, 16)) {
        Log("[ExportCurrent] Export successful!");
        std::cout << "Successfully exported sprite #" << currentSpriteIndex 
                 << " to: " << filepath << std::endl;
    } else {
        Log("[ExportCurrent] Export failed");
        std::cout << "Export FAILED." << std::endl;
    }
}

void SpriteEditor::OnFileDropped(const char* filepath) {
    Log("[DragDrop] File dropped: " + std::string(filepath));
    std::cout << "[Drag&Drop] File: " << filepath << std::endl;
    
    // Validate file extension
    std::string path(filepath);
    if (path.length() < 4 || path.substr(path.length() - 4) != ".png") {
        Log("[DragDrop] Rejected: Not a PNG file");
        std::cout << "Drop rejected: File must be a PNG" << std::endl;
        return;
    }
    
    // Validate dimensions
    int width, height;
    if (!ValidatePNG(path, width, height)) {
        std::cout << "Drop rejected: PNG must be 128x128 pixels (found: " 
                  << width << "×" << height << ")" << std::endl;
        return;
    }
    
    // Auto-import if valid
    if (ImportSpritesheet(path)) {
        AddToRecentFiles(path);
        Log("[DragDrop] Import successful!");
        std::cout << "✓ Drag & Drop Import successful: " << filepath << std::endl;
    } else {
        Log("[DragDrop] Import failed");
        std::cout << "✗ Drag & Drop Import FAILED" << std::endl;
    }
}

// ===== Recent Files Management =====

void SpriteEditor::AddToRecentFiles(const std::string& filepath) {
    // Remove if already exists (to move to front)
    auto it = std::find(recentFiles.begin(), recentFiles.end(), filepath);
    if (it != recentFiles.end()) {
        recentFiles.erase(it);
    }
    
    // Add to front
    recentFiles.insert(recentFiles.begin(), filepath);
    
    // Keep only MAX_RECENT_FILES
    if (recentFiles.size() > MAX_RECENT_FILES) {
        recentFiles.resize(MAX_RECENT_FILES);
    }
    
    Log("[RecentFiles] Added: " + filepath);
}

void SpriteEditor::LoadRecentFiles() {
    std::ifstream file("config/sprite_editor_recent.txt");
    if (!file.is_open()) {
        Log("[RecentFiles] No recent files found");
        return;
    }
    
    recentFiles.clear();
    std::string line;
    while (std::getline(file, line) && recentFiles.size() < MAX_RECENT_FILES) {
        if (!line.empty()) {
            recentFiles.push_back(line);
        }
    }
    
    file.close();
    Log("[RecentFiles] Loaded " + std::to_string(recentFiles.size()) + " recent files");
}

void SpriteEditor::SaveRecentFiles() {
    // Create config directory if it doesn't exist
    std::filesystem::create_directory("config");
    
    std::ofstream file("config/sprite_editor_recent.txt");
    if (!file.is_open()) {
        Log("[RecentFiles] Failed to save recent files");
        return;
    }
    
    for (const auto& path : recentFiles) {
        file << path << "\n";
    }
    
    file.close();
    Log("[RecentFiles] Saved " + std::to_string(recentFiles.size()) + " recent files");
}
