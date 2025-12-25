#include "ui/SpriteEditor.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "ui/UISystem.h"
#include "ui/SystemSprites.h"
#include "ui/SystemColors.h"  // Fixed UI colors
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
    , selectedColor(7)  // White
    , isActive(false)
    , zoom(CANVAS_ZOOM)  // PICO-8 style: 16x zoom = 128x128 display
    , showGrid(true)  // Grid visible by default
    , filledRectMode(false)  // Outline by default
    , currentTab(0)  // Start on tab 0 (sprites 0-63) - Task 3.16
    , isDragging(false)
    , dragStartX(0)
    , dragStartY(0)
    , hasClipboardData(false)  // No clipboard data initially
    , systemSprites(nullptr)
    , aestheticLayer(nullptr)  // Will be set in Render
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
    
    // Initialize all sprite flags to 0 (all flags off)
    std::memset(spriteFlags, 0, sizeof(spriteFlags));
    
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

void SpriteEditor::Initialize(const std::string& path, AestheticLayer* renderer) {
    spritesheetPath = path;
    
    // Store renderer reference for palette loading (Task 3.14)
    if (renderer) {
        aestheticLayer = renderer;
    }
    
    LoadSpritesheet();
    LoadSpriteFlags();  // Load sprite flags from .flags file if exists
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
            // Save history BEFORE drawing the line/rect
            SaveHistoryState();
            
            if (currentTool == Tool::LINE) {
                UseLine(dragStartX, dragStartY, canvasX, canvasY);
            } else if (currentTool == Tool::RECT) {
                UseRect(dragStartX, dragStartY, canvasX, canvasY, filledRectMode);  // Use toggle
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
            // Check palette Reset/Import/Export buttons
            else if (mouseX >= PALETTE_BUTTON_X && mouseX < PALETTE_BUTTON_X + PALETTE_BUTTON_SIZE) {
                // Button 0: Reset (Y: PALETTE_BUTTON_Y to PALETTE_BUTTON_Y + 16)
                // Button 1: Import (Y: PALETTE_BUTTON_Y + 16 to PALETTE_BUTTON_Y + 32)
                // Button 2: Export (Y: PALETTE_BUTTON_Y + 32 to PALETTE_BUTTON_Y + 48)
                if (mouseY >= PALETTE_BUTTON_Y && mouseY < PALETTE_BUTTON_Y + PALETTE_BUTTON_SIZE) {
                    HandlePaletteButtonClick(0);  // Reset
                }
                else if (mouseY >= PALETTE_BUTTON_Y + PALETTE_BUTTON_SIZE && mouseY < PALETTE_BUTTON_Y + (2 * PALETTE_BUTTON_SIZE)) {
                    HandlePaletteButtonClick(1);  // Import
                }
                else if (mouseY >= PALETTE_BUTTON_Y + (2 * PALETTE_BUTTON_SIZE) && mouseY < PALETTE_BUTTON_Y + (3 * PALETTE_BUTTON_SIZE)) {
                    HandlePaletteButtonClick(2);  // Export
                }
            }
            // Check TAB SELECTOR area (Task 3.16)
            else if (mouseY >= TAB_SELECTOR_Y && mouseY < TAB_SELECTOR_Y + TAB_BUTTON_SIZE) {
                for (int i = 0; i < NUM_TABS; i++) {
                    int tabX = CANVAS_X + (i * TAB_BUTTON_SIZE);  // No spacing - side by side
                    if (mouseX >= tabX && mouseX < tabX + TAB_BUTTON_SIZE) {
                        currentTab = i;
                        Log("[TabClick] Switched to tab " + std::to_string(i + 1));
                        break;
                    }
                }
            }
            // Check spritesheet area
            else if (mouseX >= SHEET_X && mouseX < SHEET_X + (SHEET_COLS * SHEET_SPRITE_SIZE) &&
                     mouseY >= SHEET_Y && mouseY < SHEET_Y + (SHEET_ROWS * SHEET_SPRITE_SIZE)) {
                int col = (mouseX - SHEET_X) / SHEET_SPRITE_SIZE;
                int row = (mouseY - SHEET_Y) / SHEET_SPRITE_SIZE;
                // Calculate sprite index with tab offset (Task 3.16)
                int localIndex = row * SHEET_COLS + col;  // 0-63 within tab
                int newIndex = (currentTab * SPRITES_PER_TAB) + localIndex;  // Global index (0-255)
                if (newIndex >= 0 && newIndex < 256) {
                    SwitchSprite(newIndex);
                }
            }
            // Check utility bar (vertical, right of canvas)
            else if (mouseX >= UTILITY_BAR_X && mouseX < UTILITY_BAR_X + UTILITY_BUTTON_SIZE &&
                     mouseY >= UTILITY_BAR_Y && mouseY < UTILITY_BAR_Y + (8 * UTILITY_BUTTON_SIZE)) {  // 8 botones
                HandleToolbarClick(mouseX, mouseY);  // This handles both toolbar and utility bar
            }
            // Check flag panel area (8 checkboxes below spritesheet)
            else if (mouseY >= FLAG_PANEL_Y && mouseY < FLAG_PANEL_Y + 12 &&
                     mouseX >= FLAG_PANEL_X && mouseX < FLAG_PANEL_X + (NUM_FLAGS * FLAG_CHECKBOX_SPACING)) {
                HandleFlagClick(mouseX, mouseY);
            }
            // Check toolbar area
            else if (mouseY >= TOOLBAR_Y) {
                HandleToolbarClick(mouseX, mouseY);
            }
        }
    }
}

void SpriteEditor::Render(AestheticLayer& renderer, InputManager& input) {
    if (!isActive) return;
    
    // Save aestheticLayer pointer for palette access
    aestheticLayer = &renderer;
    
    // === Constants ===
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    // Colors now come from SystemColors RGB
    
    // Bar heights (from class constants)
    const int TITLE_BAR_H = SpriteEditor::TITLE_BAR_H;
    const int STATUS_BAR_H = SpriteEditor::STATUS_BAR_H;
    
    // Clear screen with black background (RGB fixed)
    renderer.RectFillRGB(0, 0, SCREEN_W, SCREEN_H,
                         SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // === TOP BAR (LIGHT GRAY background, BLACK text - RGB fixed) ===
    renderer.RectFillRGB(0, 0, SCREEN_W, TITLE_BAR_H,
                         SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    
    renderer.PrintRGB("SPRITES", 4, 1,
                     SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    char spriteNum[16];
    sprintf(spriteNum, "#%03d", currentSpriteIndex);
    int numX = SCREEN_W - (strlen(spriteNum) * 8) - 4;
    renderer.PrintRGB(spriteNum, numX, 1,
                     SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // Render palette (uses RectFillRGB for colors)
    RenderPalette(renderer);
    
    // Render main canvas
    RenderCanvas(renderer);
    
    // Render spritesheet grid
    RenderSpritesheet(renderer);
    
    // Render flag panel (8 checkboxes for current sprite)
    RenderFlagPanel(renderer);
    
    // Render toolbar
    RenderToolbar(renderer);
    
    // Render utility bar (vertical icons)
    RenderUtilityBar(renderer);
    
    // Render cursor highlight
    RenderCursorHighlight(renderer, input);
    
    // Render drag preview (line/rect)
    if (isDragging) {
        RenderDragPreview(renderer, input);
    }
    
    // === STATUS BAR (LIGHT GRAY background, BLACK text - RGB fixed) ===
    int statusY = SCREEN_H - STATUS_BAR_H;
    renderer.RectFillRGB(0, statusY, SCREEN_W, STATUS_BAR_H,
                         SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    
    // Tool name + selected color (left side)
    const char* toolNames[] = { "PENCIL", "FILL", "LINE", "RECT", "PICKER" };
    char status[64];
    sprintf(status, "%s C:%d", toolNames[static_cast<int>(currentTool)], selectedColor);
    renderer.PrintRGB(status, 2, statusY + 1,
                     SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // Undo stack info (right side)
    if (!undoStack.empty()) {
        char undoInfo[16];
        sprintf(undoInfo, "U:%d", static_cast<int>(undoStack.size()));
        int undoX = SCREEN_W - (strlen(undoInfo) * 8) - 2;
        renderer.PrintRGB(undoInfo, undoX, statusY + 1,
                         SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
    }
}

// ===== Rendering Methods =====

void SpriteEditor::RenderCanvas(AestheticLayer& renderer) {
    // Canvas background (black, RGB fixed - not affected by palette)
    renderer.RectFillRGB(CANVAS_X, CANVAS_Y, CANVAS_SIZE, CANVAS_SIZE,
                         SystemColors::UI_CANVAS_BG.r, SystemColors::UI_CANVAS_BG.g, SystemColors::UI_CANVAS_BG.b);
    
    // Draw pixels with 16x zoom (uses editable palette for sprites)
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
    
    // Draw grid (toggle with 'G' key) - gray, RGB fixed
    if (showGrid) {
        for (int i = 0; i <= 8; i++) {
            // Vertical lines
            int x = CANVAS_X + (i * CANVAS_ZOOM);
            renderer.LineRGB(x, CANVAS_Y, x, CANVAS_Y + CANVAS_SIZE - 1,
                           SystemColors::UI_GRID.r, SystemColors::UI_GRID.g, SystemColors::UI_GRID.b);
            
            // Horizontal lines
            int y = CANVAS_Y + (i * CANVAS_ZOOM);
            renderer.LineRGB(CANVAS_X, y, CANVAS_X + CANVAS_SIZE - 1, y,
                           SystemColors::UI_GRID.r, SystemColors::UI_GRID.g, SystemColors::UI_GRID.b);
        }
    }
    
    // Canvas border (white, RGB fixed)
    renderer.RectRGB(CANVAS_X - 1, CANVAS_Y - 1, CANVAS_SIZE + 2, CANVAS_SIZE + 2,
                    SystemColors::UI_BORDER_LIGHT.r, SystemColors::UI_BORDER_LIGHT.g, SystemColors::UI_BORDER_LIGHT.b);
}

void SpriteEditor::RenderPalette(AestheticLayer& renderer) {
    for (int row = 0; row < PALETTE_ROWS; row++) {
        for (int col = 0; col < PALETTE_COLS; col++) {
            int colorIndex = row * PALETTE_COLS + col;
            int x = PALETTE_X + (col * COLOR_BOX_SIZE);
            int y = PALETTE_Y + (row * COLOR_BOX_SIZE);
            
            // Draw color box with RGB directly from palette
            // This bypasses the palette system so it always shows the correct colors
            if (aestheticLayer) {
                SDL_Color color = aestheticLayer->GetPaletteColor(colorIndex);
                renderer.RectFillRGB(x, y, COLOR_BOX_SIZE, COLOR_BOX_SIZE, color.r, color.g, color.b);
            } else {
                // Fallback: draw with indexed color (will use current palette mode)
                renderer.RectFill(x, y, COLOR_BOX_SIZE, COLOR_BOX_SIZE, colorIndex);
            }
            
            // Highlight selected color with white border
            if (colorIndex == selectedColor) {
                renderer.RectRGB(x, y, COLOR_BOX_SIZE, COLOR_BOX_SIZE,
                                SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
            }
        }
    }
    
    // White border around entire palette (matching canvas style)
    renderer.RectRGB(PALETTE_X - 1, PALETTE_Y - 1, 
                    (PALETTE_COLS * COLOR_BOX_SIZE) + 2, 
                    (PALETTE_ROWS * COLOR_BOX_SIZE) + 2,
                    SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // === Palette Reset/Import/Export Buttons (vertical, right of palette) ===
    const int buttonIcons[] = { 25, 6, 5 };  // 25 = Reset, 6 = Import, 5 = Export
    
    for (int i = 0; i < 3; i++) {
        int x = PALETTE_BUTTON_X;
        int y = PALETTE_BUTTON_Y + (i * PALETTE_BUTTON_SIZE);  // Sin spacing - pegados
        
        // Fondo negro
        renderer.RectFill(x, y, PALETTE_BUTTON_SIZE, PALETTE_BUTTON_SIZE, 0);
        
        // Fondo gris interior (RGB fixed)
        renderer.RectFillRGB(x + 1, y + 1, PALETTE_BUTTON_SIZE - 2, PALETTE_BUTTON_SIZE - 2,
                            SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
        
        // Bordes 3D (RGB fixed)
        renderer.LineRGB(x + 1, y + 1, x + 1, y + 13, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        renderer.LineRGB(x + 1, y + 1, x + 13, y + 1, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        renderer.LineRGB(x + 14, y + 1, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        renderer.LineRGB(x + 1, y + 14, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        
        // Icono
        if (systemSprites) {
            systemSprites->DrawSprite(renderer, buttonIcons[i], x + 4, y + 4, 1);
        }
    }
    
    // White border around all three buttons (RGB fixed)
    renderer.RectRGB(PALETTE_BUTTON_X - 1, PALETTE_BUTTON_Y - 1, 
                    PALETTE_BUTTON_SIZE + 2, (3 * PALETTE_BUTTON_SIZE) + 2,
                    SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
}

void SpriteEditor::RenderSpritesheet(AestheticLayer& renderer) {
    // Background (black, RGB fixed)
    int sheetW = SHEET_COLS * 8;  // 16 * 8 = 128
    int sheetH = SHEET_ROWS * 8;  // 4 * 8 = 32 (changed from 8 rows to 4)
    renderer.RectFillRGB(SHEET_X, SHEET_Y, sheetW, sheetH,
                         SystemColors::UI_CANVAS_BG.r, SystemColors::UI_CANVAS_BG.g, SystemColors::UI_CANVAS_BG.b);
    
    // Calculate offset based on current tab (Task 3.16)
    // Tab 0: sprites 0-63, Tab 1: 64-127, Tab 2: 128-191, Tab 3: 192-255
    int tabOffset = currentTab * SPRITES_PER_TAB;
    
    // Draw all visible sprites in current tab (uses editable palette)
    for (int row = 0; row < SHEET_ROWS; row++) {
        for (int col = 0; col < SHEET_COLS; col++) {
            int localIndex = row * SHEET_COLS + col;  // 0-63 within tab
            int spriteIndex = tabOffset + localIndex;  // Global sprite index (0-255)
            int screenX = SHEET_X + (col * 8);
            int screenY = SHEET_Y + (row * 8);
            
            // Draw sprite pixels (all colors including 0)
            for (int py = 0; py < 8; py++) {
                for (int px = 0; px < 8; px++) {
                    uint8_t colorIndex = spriteSheet[spriteIndex][py][px];
                    renderer.RectFill(screenX + px, screenY + py, 1, 1, colorIndex);
                }
            }
            
            // Highlight currently selected sprite (yellow, RGB fixed)
            if (spriteIndex == currentSpriteIndex) {
                renderer.RectRGB(screenX - 1, screenY - 1, 10, 10,
                                SystemColors::UI_HIGHLIGHT.r, SystemColors::UI_HIGHLIGHT.g, SystemColors::UI_HIGHLIGHT.b);
            }
        }
    }
    
    // Draw grid outline (gray, RGB fixed)
    renderer.RectRGB(SHEET_X - 1, SHEET_Y - 1, sheetW + 2, sheetH + 2,
                    SystemColors::UI_BORDER_MEDIUM.r, SystemColors::UI_BORDER_MEDIUM.g, SystemColors::UI_BORDER_MEDIUM.b);
    
    // === TAB SELECTOR (Task 3.16) ===
    // Draw 4 tab buttons styled EXACTLY like utility icons (16x16 with proper borders)
    
    for (int i = 0; i < NUM_TABS; i++) {
        int tabX = CANVAS_X + (i * TAB_BUTTON_SIZE);  // No spacing - side by side
        int tabY = TAB_SELECTOR_Y;
        
        // 1. OUTER BLACK BORDER (1px) - matches icon-bg.png
        renderer.RectFill(tabX, tabY, TAB_BUTTON_SIZE, TAB_BUTTON_SIZE, 0);  // Black background
        
        // 2. INNER BACKGROUND (14x14 gray or green) - leaves 1px black border
        SDL_Color bgColor = (i == currentTab) ? SystemColors::GREEN : SystemColors::DARK_GRAY;
        renderer.RectFillRGB(tabX + 1, tabY + 1, TAB_BUTTON_SIZE - 2, TAB_BUTTON_SIZE - 2, 
                            bgColor.r, bgColor.g, bgColor.b);
        
        // 3. 3D BORDERS - EXACTLY like utility icons
        // Left/Top = Lavender (light purple)
        renderer.LineRGB(tabX + 1, tabY + 1, tabX + 1, tabY + 13, 
                        SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);  // Left
        renderer.LineRGB(tabX + 1, tabY + 1, tabX + 13, tabY + 1, 
                        SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);  // Top
        
        // Right/Bottom = Dark Blue (shadow)
        renderer.LineRGB(tabX + 14, tabY + 1, tabX + 14, tabY + 14, 
                        SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);  // Right
        renderer.LineRGB(tabX + 1, tabY + 14, tabX + 14, tabY + 14, 
                        SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);  // Bottom
        
        // 4. TAB NUMBER (1-4) - PERFECTLY CENTERED
        char tabLabel[4];
        sprintf(tabLabel, "%d", i + 1);
        
        // Font is 8x8, single digit character
        const int charWidth = 8;
        const int charHeight = 8;
        
        // Perfect centering: (iconSize / 2) - (charSize / 2)
        int textX = tabX + (TAB_BUTTON_SIZE / 2) - (charWidth / 2);   // 8 - 4 = 4
        int textY = tabY + (TAB_BUTTON_SIZE / 2) - (charHeight / 2);  // 8 - 4 = 4
        
        renderer.PrintRGB(tabLabel, textX, textY,
                        SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    }
    
    // WHITE OUTER BORDER around all four tabs together
    int totalTabWidth = NUM_TABS * TAB_BUTTON_SIZE;  // 4 * 16 = 64
    renderer.RectRGB(CANVAS_X - 1, TAB_SELECTOR_Y - 1, 
                    totalTabWidth + 2, TAB_BUTTON_SIZE + 2,
                    SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);



}

void SpriteEditor::RenderToolbar(AestheticLayer& renderer) {
    const int BUTTON_SIZE = 16;
    
    // 8 botones: Pencil, Fill, Line, Rect, FilledRect, Picker, Import, Export
    const int toolIcons[] = { 0, 1, 2, 3, 17, 4, 6, 5 };
    // Tool types: 0-5 are tools, 6-7 are file buttons
    
    for (int i = 0; i < 8; i++) {
        int x = CANVAS_X + (i * BUTTON_SIZE);  // Sin spacing - pegados
        int y = TOOLBAR_Y;
        
        // Fondo completo negro (16x16)
        renderer.RectFill(x, y, BUTTON_SIZE, BUTTON_SIZE, 0);  // 0 = black
        
        // Determinar color de fondo RGB
        SDL_Color bgColor = SystemColors::DARK_GRAY;
        
        // Resaltar tool seleccionado (solo para tools 0-5)
        if (i < 6) {
            // Los primeros 5 son tools normales (0-4)
            // FilledRect (i=4) usa currentTool==3 (RECT) + filledRectMode
            int toolIndex = i;
            if (i == 4) toolIndex = 3;  // FilledRect usa RECT tool
            if (i == 5) toolIndex = 4;  // Picker
            
            if (static_cast<int>(currentTool) == toolIndex) {
                // Si es RECT, distinguir entre filled y outline
                if (toolIndex == 3) {
                    if ((i == 3 && !filledRectMode) || (i == 4 && filledRectMode)) {
                        bgColor = SystemColors::GREEN;  // Verde = Seleccionado
                    }
                } else {
                    bgColor = SystemColors::GREEN;  // Verde = Seleccionado
                }
            }
        }
        
        renderer.RectFillRGB(x + 1, y + 1, BUTTON_SIZE - 2, BUTTON_SIZE - 2, bgColor.r, bgColor.g, bgColor.b);
        
        // Borde lila/indigo: lado IZQUIERDO y SUPERIOR (efecto 3D - sombra)
        renderer.LineRGB(x + 1, y + 1, x + 1, y + 13, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        renderer.LineRGB(x + 1, y + 1, x + 13, y + 1, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        
        // Sombra inferior y derecha (más oscuro)
        renderer.LineRGB(x + 14, y + 1, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        renderer.LineRGB(x + 1, y + 14, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        
        // Icono blanco centrado
        if (systemSprites) {
            systemSprites->DrawSprite(renderer, toolIcons[i], x + 4, y + 4, 1);
        }
    }
    
    // White border around entire toolbar
    const int TOOLBAR_WIDTH = 8 * BUTTON_SIZE;  // 8 buttons * 16px = 128px
    renderer.RectRGB(CANVAS_X - 1, TOOLBAR_Y - 1, 
                    TOOLBAR_WIDTH + 2, BUTTON_SIZE + 2, 
                    SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
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
    
    // === HORIZONTAL TOOLBAR (bottom) - 8 botones pegados ===
    if (mouseY >= TOOLBAR_Y && mouseY < TOOLBAR_Y + BUTTON_SIZE) {
        for (int i = 0; i < 8; i++) {
            int buttonX = CANVAS_X + (i * BUTTON_SIZE);
            if (mouseX >= buttonX && mouseX < buttonX + BUTTON_SIZE) {
                // Botones 0-5: Tools
                if (i == 0) currentTool = Tool::PENCIL;
                else if (i == 1) currentTool = Tool::FILL;
                else if (i == 2) currentTool = Tool::LINE;
                else if (i == 3) {
                    currentTool = Tool::RECT;
                    filledRectMode = false;  // Rect outline
                }
                else if (i == 4) {
                    currentTool = Tool::RECT;
                    filledRectMode = true;  // Rect filled
                }
                else if (i == 5) currentTool = Tool::PICKER;
                // Botones 6-7: File
                else if (i == 6) {
                    Log("[HandleToolbarClick] Import button clicked");
                    OnImportClicked();
                }
                else if (i == 7) {
                    Log("[HandleToolbarClick] Export button clicked");
                    OnExportClicked();
                }
                return;
            }
        }
    }
    
    // === VERTICAL UTILITY BAR (right of canvas) ===
    if (mouseX >= UTILITY_BAR_X && mouseX < UTILITY_BAR_X + UTILITY_BUTTON_SIZE) {
        for (int i = 0; i < 8; i++) {  // 8 botones: Grid, FlipH, FlipV, Clear, Copy, Paste, RotLeft, RotRight
            int buttonY = UTILITY_BAR_Y + (i * UTILITY_BUTTON_SIZE);  // Sin spacing
            if (mouseY >= buttonY && mouseY < buttonY + UTILITY_BUTTON_SIZE) {
                HandleUtilityButtonClick(i);
                return;
            }
        }
    }
}

void SpriteEditor::HandleUtilityButtonClick(int index) {
    switch (index) {
        case 0: // Grid toggle
            showGrid = !showGrid;
            Log(showGrid ? "[Grid] Enabled" : "[Grid] Disabled");
            break;
        case 1: // Flip horizontal (era caso 2, ahora 1)
            SaveHistoryState();
            MirrorHorizontal();
            break;
        case 2: // Flip vertical (era caso 3, ahora 2)
            SaveHistoryState();
            MirrorVertical();
            break;
        case 3: // Clear canvas (era caso 4, ahora 3)
            SaveHistoryState();
            ClearCanvas();
            Log("[Clear] Canvas cleared");
            break;
        case 4: // Copy sprite (era caso 5, ahora 4)
            std::memcpy(clipboard, canvas, sizeof(clipboard));
            hasClipboardData = true;
            Log("[Clipboard] Sprite copied");
            break;
        case 5: // Paste sprite (era caso 6, ahora 5)
            if (hasClipboardData) {
                SaveHistoryState();
                std::memcpy(canvas, clipboard, sizeof(canvas));
                Log("[Clipboard] Sprite pasted");
            } else {
                Log("[Clipboard] No data to paste");
            }
            break;
        case 6: // Rotate Left (counter-clockwise)
            SaveHistoryState();
            RotateLeft();
            Log("[Transform] Rotate left");
            break;
        case 7: // Rotate Right (clockwise)
            SaveHistoryState();
            RotateRight();
            Log("[Transform] Rotate right");
            break;
    }
}

void SpriteEditor::HandleKeyboard(InputManager& input) {
    // Tool shortcuts
    if (input.isKeyPressed(SDL_SCANCODE_P)) currentTool = Tool::PENCIL;
    if (input.isKeyPressed(SDL_SCANCODE_F)) currentTool = Tool::FILL;
    if (input.isKeyPressed(SDL_SCANCODE_L)) currentTool = Tool::LINE;
    if (input.isKeyPressed(SDL_SCANCODE_R)) currentTool = Tool::RECT;
    if (input.isKeyPressed(SDL_SCANCODE_C)) currentTool = Tool::PICKER;
    
    // Grid toggle (G)
    if (input.isKeyPressed(SDL_SCANCODE_G)) {
        showGrid = !showGrid;
        Log(showGrid ? "[Grid] Enabled" : "[Grid] Disabled");
    }
    
    
    // Check for modifier keys (used by multiple hotkeys)
    bool ctrlHeld = input.isKeyDown(SDL_SCANCODE_LCTRL) || input.isKeyDown(SDL_SCANCODE_RCTRL);
    bool shiftHeld = input.isKeyDown(SDL_SCANCODE_LSHIFT) || input.isKeyDown(SDL_SCANCODE_RSHIFT);
    
    // Filled rectangle mode toggle (X)
    if (input.isKeyPressed(SDL_SCANCODE_X)) {
        filledRectMode = !filledRectMode;
        Log(filledRectMode ? "[Rect] Filled mode" : "[Rect] Outline mode");
    }
    
    // Mirror/Flip tools (H = Horizontal, Shift+H = Vertical)
    if (input.isKeyPressed(SDL_SCANCODE_H)) {
        SaveHistoryState();  // Save before transforming
        if (shiftHeld) {
            MirrorVertical();
        } else {
            MirrorHorizontal();
        }
    }
    
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
        // Clear canvas (Ctrl+N)
        if (input.isKeyPressed(SDL_SCANCODE_N)) {
            SaveHistoryState();  // Save before clearing
            ClearCanvas();
            Log("[Clear] Canvas cleared");
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
        // Copy sprite to clipboard (Ctrl+C)
        if (input.isKeyPressed(SDL_SCANCODE_C)) {
            std::memcpy(clipboard, canvas, sizeof(clipboard));
            hasClipboardData = true;
            Log("[Clipboard] Sprite copied");
        }
        // Paste sprite from clipboard (Ctrl+V)
        if (input.isKeyPressed(SDL_SCANCODE_V)) {
            if (hasClipboardData) {
                SaveHistoryState();  // Save before pasting
                std::memcpy(canvas, clipboard, sizeof(canvas));
                Log("[Clipboard] Sprite pasted");
            } else {
                Log("[Clipboard] No data to paste");
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
    
    // Auto-switch tab if sprite is in a different tab (Task 3.16)
    // Tab 0: 0-63, Tab 1: 64-127, Tab 2: 128-191, Tab 3: 192-255
    int requiredTab = currentSpriteIndex / SPRITES_PER_TAB;
    if (requiredTab != currentTab) {
        currentTab = requiredTab;
        Log("[AutoSwitch] Changed to tab " + std::to_string(currentTab + 1) + " for sprite #" + std::to_string(currentSpriteIndex));
    }
    
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
        
        // Auto-save palette.pal to cartridge folder
        SaveCartridgePalette();
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
    
    // Auto-load palette.pal from cartridge folder
    LoadCartridgePalette();
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
// Cursor Highlight and Drag Preview Implementation

void SpriteEditor::RenderCursorHighlight(AestheticLayer& renderer, InputManager& input) {
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    // Only highlight if mouse is over canvas
    if (mouseX >= CANVAS_X && mouseX < CANVAS_X + CANVAS_SIZE &&
        mouseY >= CANVAS_Y && mouseY < CANVAS_Y + CANVAS_SIZE) {
        
        // Calculate which pixel the mouse is over
        int canvasX = (mouseX - CANVAS_X) / CANVAS_ZOOM;
        int canvasY = (mouseY - CANVAS_Y) / CANVAS_ZOOM;
        
        if (canvasX >= 0 && canvasX < 8 && canvasY >= 0 && canvasY < 8) {
            int screenX = CANVAS_X + (canvasX * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (canvasY * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                           SystemColors::UI_BORDER_LIGHT.r, SystemColors::UI_BORDER_LIGHT.g, SystemColors::UI_BORDER_LIGHT.b);
        }
    }
}

void SpriteEditor::RenderDragPreview(AestheticLayer& renderer, InputManager& input) {
    if (!isDragging) return;
    
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    int endCanvasX = ScreenToCanvasX(mouseX);
    int endCanvasY = ScreenToCanvasY(mouseY);
    
    if (!IsInCanvas(endCanvasX, endCanvasY)) return;
    
    // Preview color from SystemColors
    SDL_Color previewColor = SystemColors::YELLOW;
    
    if (currentTool == Tool::LINE) {
        //  Preview de línea usando Bresenham
        int x1 = dragStartX, y1 = dragStartY;
        int x2 = endCanvasX, y2 = endCanvasY;
        
        // Bresenham's line algorithm
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        int x = x1, y = y1;
        while (true) {
            // Draw preview pixel (no fill, just outline)
            int screenX = CANVAS_X + (x * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (y * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                            previewColor.r, previewColor.g, previewColor.b);
            
            if (x == x2 && y == y2) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x += sx; }
            if (e2 < dx) { err += dx; y += sy; }
        }
    }
    else if (currentTool == Tool::RECT) {
        // Preview de rectángulo (solo outline)
        int minX = std::min(dragStartX, endCanvasX);
        int maxX = std::max(dragStartX, endCanvasX);
        int minY = std::min(dragStartY, endCanvasY);
        int maxY = std::max(dragStartY, endCanvasY);
        
        // Draw preview rectangle outline
        for (int x = minX; x <= maxX; x++) {
            // Top edge
            int screenX = CANVAS_X + (x * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (minY * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                            previewColor.r, previewColor.g, previewColor.b);
            
            // Bottom edge
            screenY = CANVAS_Y + (maxY * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                            previewColor.r, previewColor.g, previewColor.b);
        }
        
        for (int y = minY + 1; y < maxY; y++) {
            // Left edge
            int screenX = CANVAS_X + (minX * CANVAS_ZOOM);
            int screenY = CANVAS_Y + (y * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                            previewColor.r, previewColor.g, previewColor.b);
            
            // Right edge
            screenX = CANVAS_X + (maxX * CANVAS_ZOOM);
            renderer.RectRGB(screenX, screenY, CANVAS_ZOOM, CANVAS_ZOOM,
                            previewColor.r, previewColor.g, previewColor.b);
        }
    }
}
// Transform Tools Implementation

void SpriteEditor::MirrorHorizontal() {
    // Flip sprite left-right
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 4; x++) {  // Only need to swap half
            uint8_t temp = canvas[y][x];
            canvas[y][x] = canvas[y][7 - x];
            canvas[y][7 - x] = temp;
        }
    }
    Log("[Transform] Mirror horizontal");
}

void SpriteEditor::MirrorVertical() {
    // Flip sprite top-bottom
    for (int y = 0; y < 4; y++) {  // Only need to swap half
        for (int x = 0; x < 8; x++) {
            uint8_t temp = canvas[y][x];
            canvas[y][x] = canvas[7 - y][x];
            canvas[7 - y][x] = temp;
        }
    }
    Log("[Transform] Mirror vertical");
}

void SpriteEditor::RotateLeft() {
    // Rotate 90° counter-clockwise
    uint8_t temp[8][8];
    
    // Copy to temp with rotation
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            temp[7 - x][y] = canvas[y][x];
        }
    }
    
    // Copy back
    std::memcpy(canvas, temp, sizeof(canvas));
    Log("[Transform] Rotate left");
}

void SpriteEditor::RotateRight() {
    // Rotate 90° clockwise
    uint8_t temp[8][8];
    
    // Copy to temp with rotation
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            temp[x][7 - y] = canvas[y][x];
        }
    }
    
    // Copy back
    std::memcpy(canvas, temp, sizeof(canvas));
    Log("[Transform] Rotate right");
}

void SpriteEditor::RenderUtilityBar(AestheticLayer& renderer) {
    // 8 iconos: Grid, FlipH, FlipV, Clear, Copy, Paste, RotateLeft, RotateRight
    const int utilityIcons[] = { 16, 18, 19, 20, 21, 22, 23, 24 };
    
    for (int i = 0; i < 8; i++) {  // 8 botones ahora
        int x = UTILITY_BAR_X;
        int y = UTILITY_BAR_Y + (i * UTILITY_BUTTON_SIZE);  // Sin spacing - pegados
        
        // Fondo completo negro (16x16)
        renderer.RectFill(x, y, UTILITY_BUTTON_SIZE, UTILITY_BUTTON_SIZE, 0);  // 0 = black
        
        // Fondo gris interior (14x14, dejando 1px de cada lado)
        SDL_Color bgColor = SystemColors::DARK_GRAY;
        if (i == 0) {  // Grid toggle button
            bgColor = showGrid ? SystemColors::GREEN : SystemColors::DARK_GRAY;  // Verde si Grid activo
        }
        renderer.RectFillRGB(x + 1, y + 1, UTILITY_BUTTON_SIZE - 2, UTILITY_BUTTON_SIZE - 2, bgColor.r, bgColor.g, bgColor.b);
        
        // Borde lila/indigo: lado IZQUIERDO y SUPERIOR (efecto 3D - sombra)
        renderer.LineRGB(x + 1, y + 1, x + 1, y + 13, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);  // Lado izquierdo
        renderer.LineRGB(x + 1, y + 1, x + 13, y + 1, SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);  // Lado superior
        
        // Sombra (lado derecho e inferior)
        renderer.LineRGB(x + 14, y + 1, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);  // Lado derecho
        renderer.LineRGB(x + 1, y + 14, x + 14, y + 14, SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);  // Lado inferior
        
        // Icono blanco centrado
        if (systemSprites) {
            systemSprites->DrawSprite(renderer, utilityIcons[i], x + 4, y + 4, 1);
        }
    }
    
    // White border around entire utility bar
    const int UTILITY_BAR_HEIGHT = 8 * UTILITY_BUTTON_SIZE;  // 8 buttons * 16px = 128px
    renderer.RectRGB(UTILITY_BAR_X - 1, UTILITY_BAR_Y - 1, 
                    UTILITY_BUTTON_SIZE + 2, UTILITY_BAR_HEIGHT + 2,
                    SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
}

void SpriteEditor::HandlePaletteButtonClick(int buttonIndex) {
    Log("[Palette] Button clicked: " + std::to_string(buttonIndex));
    
    switch (buttonIndex) {
        case 0:
            // Reset palette to default
            ResetPaletteToDefault();
            break;
        case 1:
            // Import palette
            ImportPalette();
            break;
        case 2:
            // Export palette
            ExportPalette();
            break;
    }
}

void SpriteEditor::ImportPalette() {
    Log("[Palette] Import button clicked - Opening file dialog...");
    
    // Open file dialog for .pal file
    std::string filename = FileDialog::OpenFile("Palette Files\0*.pal\0All Files\0*.*\0", "Import Palette");
    
    if (filename.empty()) {
        Log("[Palette] User cancelled import");
        return;
    }
    
    Log("[Palette] Loading palette from: " + filename);
    
    // Load palette file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        Log("[Palette] ERROR: Could not open file");
        return;
    }
    
    // Read palette data (32 colors * 3 bytes RGB = 96 bytes)
    std::vector<SDL_Color> newPalette;
    for (int i = 0; i < 32; i++) {
        uint8_t r, g, b;
        file.read(reinterpret_cast<char*>(&r), 1);
        file.read(reinterpret_cast<char*>(&g), 1);
        file.read(reinterpret_cast<char*>(&b), 1);
        
        if (file.fail()) {
            Log("[Palette] ERROR: Failed to read color " + std::to_string(i));
            file.close();
            return;
        }
        
        newPalette.push_back({r, g, b, 255});
    }
    
    file.close();
    
    // Apply ALL 32 colors from imported palette
    if (aestheticLayer) {
        aestheticLayer->LoadPalette(newPalette);
        Log("[Palette] Successfully imported 32 colors");
        
        // Auto-save to cartridge's palette.pal
        SaveCartridgePalette();
    }
}

void SpriteEditor::ExportPalette() {
    Log("[Palette] Export button clicked - Opening save dialog...");
    
    // Open save dialog for .pal file
    std::string filename = FileDialog::SaveFile("palette.pal", "Palette Files\0*.pal\0All Files\0*.*\0", "Export Palette", "pal");
    
    if (filename.empty()) {
        Log("[Palette] User cancelled export");
        return;
    }
    
    // Ensure .pal extension
    if (filename.find(".pal") == std::string::npos) {
        filename += ".pal";
    }
    
    Log("[Palette] Saving palette to: " + filename);
    
    // Save palette file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        Log("[Palette] ERROR: Could not create file");
        return;
    }
    
    // Get current palette from AestheticLayer
    if (!aestheticLayer) {
        Log("[Palette] ERROR: AestheticLayer not available");
        file.close();
        return;
    }
    
    int paletteSize = aestheticLayer->GetPaletteSize();
    Log("[Palette] Current palette size: " + std::to_string(paletteSize));
    
    // Write palette data (always export 32 colors, pad with black if needed)
    for (int i = 0; i < 32; i++) {
        SDL_Color color = aestheticLayer->GetPaletteColor(i);
        
        file.write(reinterpret_cast<const char*>(&color.r), 1);
        file.write(reinterpret_cast<const char*>(&color.g), 1);
        file.write(reinterpret_cast<const char*>(&color.b), 1);
    }
    
    file.close();
    Log("[Palette] Successfully exported 32 colors");
}

// === Auto-load/save palette from cartridge ===

void SpriteEditor::LoadCartridgePalette() {
    if (spritesheetPath.empty()) {
        Log("[Palette] No spritesheet path, using default palette");
        return;
    }
    
    // Get palette path (same folder as spritesheet)
    std::filesystem::path sheetPath(spritesheetPath);
    std::filesystem::path palettePath = sheetPath.parent_path() / "palette.pal";
    
    Log("[Palette] Looking for palette.pal at: " + palettePath.string());
    
    // Check if palette.pal exists
    if (!std::filesystem::exists(palettePath)) {
        Log("[Palette] No palette.pal found, creating default");
        SaveCartridgePalette();  // Create default palette.pal
        return;
    }
    
    // Load palette file
    std::ifstream file(palettePath, std::ios::binary);
    if (!file.is_open()) {
        Log("[Palette] ERROR: Could not open palette.pal");
        return;
    }
    
    // Read palette data (32 colors * 3 bytes RGB = 96 bytes)
    std::vector<SDL_Color> newPalette;
    for (int i = 0; i < 32; i++) {
        uint8_t r, g, b;
        file.read(reinterpret_cast<char*>(&r), 1);
        file.read(reinterpret_cast<char*>(&g), 1);
        file.read(reinterpret_cast<char*>(&b), 1);
        
        if (file.fail()) {
            Log("[Palette] ERROR: Failed to read color " + std::to_string(i));
            file.close();
            return;
        }
        
        newPalette.push_back({r, g, b, 255});
    }
    
    file.close();
    
    // === VALIDATE: Check if palette is empty (all or mostly black) ===
    int nonBlackColors = 0;
    for (const auto& color : newPalette) {
        if (color.r > 10 || color.g > 10 || color.b > 10) {
            nonBlackColors++;
        }
    }
    
    if (nonBlackColors < 4) {
        Log("[Palette] WARN: Palette appears empty (" + std::to_string(nonBlackColors) + " non-black colors), recreating");
        std::filesystem::remove(palettePath);
        SaveCartridgePalette();
        
        // Reload the newly created default palette
        LoadCartridgePalette();
        return;
    }
    
    // Apply palette to AestheticLayer
    if (aestheticLayer) {
        aestheticLayer->LoadPalette(newPalette);
        Log("[Palette] Loaded palette.pal successfully (32 colors)");
    }
}

void SpriteEditor::SaveCartridgePalette() {
    if (spritesheetPath.empty()) {
        Log("[Palette] No spritesheet path, cannot save palette");
        return;
    }
    
    // Get palette path (same folder as spritesheet)
    std::filesystem::path sheetPath(spritesheetPath);
    std::filesystem::path palettePath = sheetPath.parent_path() / "palette.pal";
    
    Log("[Palette] Saving palette.pal to: " + palettePath.string());
    
    // Save palette file
    std::ofstream file(palettePath, std::ios::binary);
    if (!file.is_open()) {
        Log("[Palette] ERROR: Could not create palette.pal");
        return;
    }
    
    // Write palette data (always export 32 colors)
    if (aestheticLayer) {
        for (int i = 0; i < 32; i++) {
            SDL_Color color = aestheticLayer->GetPaletteColor(i);
            file.write(reinterpret_cast<const char*>(&color.r), 1);
            file.write(reinterpret_cast<const char*>(&color.g), 1);
            file.write(reinterpret_cast<const char*>(&color.b), 1);
        }
        file.close();
        Log("[Palette] Saved palette.pal successfully");
    } else {
        // Write default 32-color palette (PICO-8 + TIC-80)
        const uint8_t defaultPalette[32][3] = {
            // First 16 colors (PICO-8)
            {0, 0, 0}, {29, 43, 83}, {126, 37, 83}, {0, 135, 81},
            {171, 82, 54}, {95, 87, 79}, {194, 195, 199}, {255, 241, 232},
            {255, 0, 77}, {255, 163, 0}, {255, 236, 39}, {0, 228, 54},
            {41, 173, 255}, {131, 118, 156}, {255, 119, 168}, {255, 204, 170},
            // Extended 16 colors (TIC-80)
            {26, 28, 44}, {93, 39, 93}, {177, 62, 83}, {239, 125, 87},
            {255, 205, 117}, {167, 240, 112}, {56, 183, 100}, {37, 113, 121},
            {41, 54, 111}, {59, 93, 201}, {65, 166, 246}, {115, 239, 247},
            {244, 244, 244}, {148, 176, 194}, {86, 108, 134}, {51, 60, 87}
        };
        
        for (int i = 0; i < 32; i++) {
            file.write(reinterpret_cast<const char*>(&defaultPalette[i][0]), 1);
            file.write(reinterpret_cast<const char*>(&defaultPalette[i][1]), 1);
            file.write(reinterpret_cast<const char*>(&defaultPalette[i][2]), 1);
        }
        file.close();
        Log("[Palette] Saved default 32-color palette.pal");
    }
}

void SpriteEditor::ResetPaletteToDefault() {
    Log("[Palette] Resetting to default palette...");
    
    if (!aestheticLayer) {
        Log("[Palette] ERROR: aestheticLayer is null");
        return;
    }
    
    // Reset palette to PICO-8/TIC-80 default
    aestheticLayer->ResetToDefaultPalette();
    
    // Save the default palette to the cartridge's palette.pal file
    SaveCartridgePalette();
    
    Log("[Palette] Reset to default and saved successfully");
}

// ===== SPRITE FLAGS IMPLEMENTATION =====

void SpriteEditor::RenderFlagPanel(AestheticLayer& renderer) {
    // Draw 8 checkboxes for the current sprite's flags
    // Layout: FLAGS: [0] [1] [2] [3] [4] [5] [6] [7]
    
    int y = FLAG_PANEL_Y;
    
    // Label
    renderer.PrintRGB("FLAGS:", FLAG_PANEL_X, y,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Draw 8 checkboxes
    for (int i = 0; i < NUM_FLAGS; i++) {
        int x = FLAG_PANEL_X + 40 + (i * FLAG_CHECKBOX_SPACING);
        
        // Checkbox background (dark gray)
        renderer.RectFillRGB(x, y, FLAG_CHECKBOX_SIZE, FLAG_CHECKBOX_SIZE,
                            SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
        
        // Checkbox border (white)
        renderer.RectRGB(x, y, FLAG_CHECKBOX_SIZE, FLAG_CHECKBOX_SIZE,
                        SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
        
        // If flag is set, draw checkmark (green)
        if (GetSpriteFlag(currentSpriteIndex, i)) {
            renderer.RectFillRGB(x + 2, y + 2, FLAG_CHECKBOX_SIZE - 4, FLAG_CHECKBOX_SIZE - 4,
                                SystemColors::GREEN.r, SystemColors::GREEN.g, SystemColors::GREEN.b);
        }
        
        // Flag number label below checkbox
        char label[4];
        sprintf(label, "%d", i);
        renderer.PrintRGB(label, x, y + 10,
                         SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    }
}

void SpriteEditor::HandleFlagClick(int mouseX, int mouseY) {
    // Determine which flag checkbox was clicked
    int y = FLAG_PANEL_Y;
    
    for (int i = 0; i < NUM_FLAGS; i++) {
        int x = FLAG_PANEL_X + 40 + (i * FLAG_CHECKBOX_SPACING);
        
        if (mouseX >= x && mouseX < x + FLAG_CHECKBOX_SIZE &&
            mouseY >= y && mouseY < y + FLAG_CHECKBOX_SIZE) {
            // Toggle flag
            ToggleSpriteFlag(currentSpriteIndex, i);
            SaveSpriteFlags();  // Auto-save flags
            Log("[Flags] Toggled flag " + std::to_string(i) + " for sprite #" + std::to_string(currentSpriteIndex));
            break;
        }
    }
}

// Sprite Flags API Implementation

bool SpriteEditor::GetSpriteFlag(int spriteIndex, int flagBit) const {
    if (spriteIndex < 0 || spriteIndex >= 256 || flagBit < 0 || flagBit >= 8) {
        return false;  // Out of bounds
    }
    
    return (spriteFlags[spriteIndex] & (1 << flagBit)) != 0;
}

void SpriteEditor::SetSpriteFlag(int spriteIndex, int flagBit, bool value) {
    if (spriteIndex < 0 || spriteIndex >= 256 || flagBit < 0 || flagBit >= 8) {
        return;  // Out of bounds
    }
    
    if (value) {
        spriteFlags[spriteIndex] |= (1 << flagBit);  // Set bit
    } else {
        spriteFlags[spriteIndex] &= ~(1 << flagBit);  // Clear bit
    }
}

void SpriteEditor::ToggleSpriteFlag(int spriteIndex, int flagBit) {
    if (spriteIndex < 0 || spriteIndex >= 256 || flagBit < 0 || flagBit >= 8) {
        return;  // Out of bounds
    }
    
    spriteFlags[spriteIndex] ^= (1 << flagBit);  // XOR to toggle
}

uint8_t SpriteEditor::GetSpriteFlagsAll(int spriteIndex) const {
    if (spriteIndex < 0 || spriteIndex >= 256) {
        return 0;  // Out of bounds
    }
    
    return spriteFlags[spriteIndex];
}

void SpriteEditor::SetSpriteFlagsAll(int spriteIndex, uint8_t flags) {
    if (spriteIndex < 0 || spriteIndex >= 256) {
        return;  // Out of bounds
    }
    
    spriteFlags[spriteIndex] = flags;
}

void SpriteEditor::SaveSpriteFlags() {
    if (spritesheetPath.empty()) {
        Log("[Flags] No spritesheet path set, cannot save flags");
        return;
    }
    
    // Determine .flags file path from spritesheet path
    // spritesheet.png -> spritesheet.flags
    std::string flagsPath = spritesheetPath;
    size_t dotPos = flagsPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        flagsPath = flagsPath.substr(0, dotPos) + ".flags";
    } else {
        flagsPath += ".flags";
    }
    
    // Write 256 bytes to file
    std::ofstream file(flagsPath, std::ios::binary);
    if (!file) {
        Log("[Flags] ERROR: Could not open file for writing: " + flagsPath);
        return;
    }
    
    file.write(reinterpret_cast<const char*>(spriteFlags), 256);
    file.close();
    
    Log("[Flags] Saved to: " + flagsPath);
}

void SpriteEditor::SetCartridgePath(const std::string& path) {
    // Set spritesheet path for flags loading
    // Append '/spritesheet.png' to cartridge folder path
    spritesheetPath = path + "/spritesheet.png";
    Log("[Flags] Cartridge path set: " + spritesheetPath);
}

void SpriteEditor::LoadSpriteFlags() {
    if (spritesheetPath.empty()) {
        Log("[Flags] No spritesheet path set, cannot load flags");
        return;
    }
    
    // Determine .flags file path from spritesheet path
    std::string flagsPath = spritesheetPath;
    size_t dotPos = flagsPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        flagsPath = flagsPath.substr(0, dotPos) + ".flags";
    } else {
        flagsPath += ".flags";
    }
    
    // Check if file exists
    std::ifstream file(flagsPath, std::ios::binary);
    if (!file) {
        Log("[Flags] No .flags file found, using defaults (all flags = 0)");
        std::memset(spriteFlags, 0, sizeof(spriteFlags));
        return;
    }
    
    // Read 256 bytes from file
    file.read(reinterpret_cast<char*>(spriteFlags), 256);
    file.close();
    
    Log("[Flags] Loaded from: " + flagsPath);
}
