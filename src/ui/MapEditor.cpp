#include "ui/MapEditor.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "ui/SystemSprites.h"
#include "ui/SystemColors.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <json.hpp>
#include <cassert>  // For debug assertions
#include <ctime>    // For timestamp logging
#include <filesystem>  // For path operations

using json = nlohmann::json;

MapEditor::MapEditor() 
    : activeLayer(0),
      undoIndex(-1),
      currentTool(Tool::PENCIL),
      selectedTile(1),  // Default to tile 1 (not empty)
      isDrawing(false),
      lastDrawnTileX(-1),
      lastDrawnTileY(-1),
      isFillingOperation(false),
      isActive(false),
      showGrid(true),
      cameraX(4),   // Start map at position (4,4)
      cameraY(4),
      zoom(1.0f),   // Start at 100% zoom (1x)
      isPanning(false),
      panStartX(0),
      panStartY(0),
      cameraStartX(0),
      cameraStartY(0),
      showLayerSidebar(false),
      hoveredLayer(-1),
      showTooltip(false),
      tooltipTileX(0),
      tooltipTileY(0),
      currentCursor(CursorType::ARROW),
      arrowCursor(nullptr),
      handCursor(nullptr),
      crossCursor(nullptr),
      toastMessage(""),
      toastTimer(0),
      currentTab(0),  // Start at tab 0
      systemSprites(nullptr) {
    
    // Initialize 8 layers
    layers.resize(LAYER_COUNT);
    for (int i = 0; i < LAYER_COUNT; i++) {
        layers[i].name = "Layer " + std::to_string(i);
        layers[i].visible = (i == 0);  // Only layer 0 visible by default
    }
    
    // Initialize cursors
    InitializeCursors();
    
    std::cout << "[MapEditor] Initialized with " << LAYER_COUNT << " layers" << std::endl;
}

MapEditor::~MapEditor() {
    CleanupCursors();
    std::cout << "[MapEditor] Destroyed" << std::endl;
}

void MapEditor::Initialize(const std::string& path) {
    mapPath = path;
    
    // Try to load existing map
    if (LoadFromJSON(path)) {
        std::cout << "[MapEditor] Loaded map from: " << path << std::endl;
    } else {
        std::cout << "[MapEditor] No existing map, starting fresh" << std::endl;
    }
}

void MapEditor::SetActive(bool active, AestheticLayer* renderer) {
    bool wasInactive = !isActive;
    isActive = active;
    
    if (active && renderer) {
        // Clear undo history and save initial state when opening editor  
        if (wasInactive) {
            ClearUndoHistory();
            SaveUndoState("Initial");
        }
        
        // When activating MapEditor, reload spritesheet to get latest changes from Sprite Editor
        // Deduce spritesheet path from mapPath (e.g., "cartridges/test_map/map.json" -> "cartridges/test_map/spritesheet.png")
        if (!mapPath.empty()) {
            std::filesystem::path mapFilePath(mapPath);
            std::filesystem::path cartridgePath = mapFilePath.parent_path();
            std::filesystem::path spritesheetPath = cartridgePath / "spritesheet.png";
            
            if (std::filesystem::exists(spritesheetPath)) {
                std::cout << "[MapEditor] Loading spritesheet from: " << spritesheetPath << std::endl;
                renderer->LoadSpriteSheet(spritesheetPath.string());
            } else {
                std::cout << "[MapEditor] Warning: No spritesheet.png found at " << spritesheetPath << std::endl;
            }
        }
    }
}

// ===== Core Update/Render =====

void MapEditor::Update(InputManager& input) {
    if (!isActive) return;
    
    // Update cursor based on mouse position
    UpdateCursor(input.getMouseX(), input.getMouseY());
    
    // Update toast timer
    if (toastTimer > 0) {
        toastTimer--;
    }
    
    // Handle keyboard shortcuts
    HandleKeyboard(input);
    
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    
    // Update tooltip (show tile position when hovering over map)
    if (mouseX >= MAP_X && mouseX < MAP_X + MAP_W &&
        mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H &&
        (!showLayerSidebar || mouseX >= SIDEBAR_X + SIDEBAR_W)) {
        // Mouse is over map viewport
        int tileX = ScreenToTileX(mouseX);
        int tileY = ScreenToTileY(mouseY);
        if (IsValidTileCoord(tileX, tileY)) {
            showTooltip = true;
            tooltipTileX = tileX;
            tooltipTileY = tileY;
        } else {
            showTooltip = false;
        }
    } else {
        showTooltip = false;
    }
    
    // === ZOOM with Mouse Wheel (discrete levels) ===
    int wheelDelta = input.getMouseWheelY();
    if (wheelDelta != 0 && mouseX >= MAP_X && mouseX < MAP_X + MAP_W && mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H) {
        // 5 discrete zoom levels: 0.25x, 0.5x, 1x, 2x, 4x
        float zoomLevels[] = {0.25f, 0.5f, 1.0f, 2.0f, 4.0f};
        int currentLevel = 2;  // Start at 1x
        
        // Find current zoom level
        for (int i = 0; i < 5; i++) {
            if (zoom == zoomLevels[i]) {
                currentLevel = i;
                break;
            }
        }
        
        // Wheel forward (positive) = zoom in (higher level = bigger sprites)
        // Wheel backward (negative) = zoom out (lower level = smaller sprites)
        if (wheelDelta > 0 && currentLevel < 4) {
            currentLevel++;  // Zoom IN (bigger sprites: 1x → 2x → 4x)
        } else if (wheelDelta < 0 && currentLevel > 0) {
            currentLevel--;  // Zoom OUT (smaller sprites: 1x → 0.5x → 0.25x)
        }
        
        zoom = zoomLevels[currentLevel];
        std::cout << "[MapEditor] Zoom: " << (zoom * 100) << "%" << std::endl;
    }
    
    // === MOVE MAP with Middle Mouse (like Photoshop hand tool) ===
    bool isMiddlePressed = input.isMouseButtonDown(SDL_BUTTON_MIDDLE);
    
    if (isMiddlePressed && mouseX >= MAP_X && mouseX < MAP_X + MAP_W && mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H) {
        if (!isPanning) {
            // Start dragging
            isPanning = true;
            panStartX = mouseX;
            panStartY = mouseY;
            cameraStartX = cameraX;
            cameraStartY = cameraY;
        } else {
            // Move map based on drag
            int deltaX = mouseX - panStartX;
            int deltaY = mouseY - panStartY;
            
            cameraX = cameraStartX + deltaX;
            cameraY = cameraStartY + deltaY;
        }
    } else {
        // Just released middle mouse - snap to grid
        if (isPanning) {
            // Snap to checkboard grid (8px = 1 tile)
            int gridSize = 8;
            cameraX = ((cameraX + gridSize / 2) / gridSize) * gridSize;
            cameraY = ((cameraY + gridSize / 2) / gridSize) * gridSize;
            std::cout << "[MapEditor] Snapped to grid: (" << cameraX << ", " << cameraY << ")" << std::endl;
        }
        isPanning = false;
    }
    
    // Update hover state for sidebar
    if (showLayerSidebar && mouseX >= SIDEBAR_X && mouseX < SIDEBAR_X + SIDEBAR_W &&
        mouseY >= SIDEBAR_Y + SIDEBAR_LIST_START_Y && mouseY < SIDEBAR_Y + SIDEBAR_LIST_START_Y + LAYER_COUNT * LAYER_ITEM_H) {
        hoveredLayer = (mouseY - (SIDEBAR_Y + SIDEBAR_LIST_START_Y)) / LAYER_ITEM_H;
    } else {
        hoveredLayer = -1;
    }
    
    // Handle left mouse click (start drawing)
    if (input.isMouseButtonPressed(SDL_BUTTON_LEFT) && !isPanning) {
        // Toggle button in title bar
        if (mouseX >= TOGGLE_BTN_X && mouseX < TOGGLE_BTN_X + TOGGLE_BTN_SIZE && 
            mouseY >= TOGGLE_BTN_Y && mouseY < TOGGLE_BTN_Y + TOGGLE_BTN_SIZE) {
            showLayerSidebar = !showLayerSidebar;
            std::cout << "[MapEditor] Layer sidebar: " << (showLayerSidebar ? "open" : "closed") << std::endl;
        }
        // Sidebar layer click
        else if (showLayerSidebar && mouseX >= SIDEBAR_X && mouseX < SIDEBAR_X + SIDEBAR_W &&
                 mouseY >= SIDEBAR_Y + SIDEBAR_LIST_START_Y && mouseY < SIDEBAR_Y + SIDEBAR_LIST_START_Y + LAYER_COUNT * LAYER_ITEM_H) {
            int clickedLayer = (mouseY - (SIDEBAR_Y + SIDEBAR_LIST_START_Y)) / LAYER_ITEM_H;
            
            // Click on Show/Hide text toggles visibility
            if (mouseX >= SIDEBAR_X + LAYER_VIS_X && mouseX < SIDEBAR_X + LAYER_VIS_X + LAYER_VIS_W) {
                layers[clickedLayer].visible = !layers[clickedLayer].visible;
                std::cout << "[MapEditor] Layer " << clickedLayer << " visibility: " 
                          << (layers[clickedLayer].visible ? "on" : "off") << std::endl;
            }
            // Click anywhere else selects layer
            else {
                activeLayer = clickedLayer;
                std::cout << "[MapEditor] Active layer: " << activeLayer << std::endl;
            }
        }
        // Map viewport - start drawing
        else if (mouseX >= MAP_X && mouseX < MAP_X + MAP_W &&
                 mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H) {
            // Only handle if not clicking on sidebar
            if (!showLayerSidebar || mouseX >= SIDEBAR_X + SIDEBAR_W) {
                isDrawing = true;
                lastDrawnTileX = -1;  // Reset to allow first paint
                lastDrawnTileY = -1;
                HandleViewportClick(mouseX, mouseY);
            }
        }
        // Spritesheet
        else if (mouseX >= SHEET_X && mouseX < SHEET_X + SHEET_W &&
                 mouseY >= SHEET_Y && mouseY < SHEET_Y + SHEET_H) {
            HandlePickerClick(mouseX, mouseY);
        }
        // Toolbar
        else if (mouseY >= TOOLBAR_Y && mouseY < TOOLBAR_Y + TOOLBAR_H) {
            HandleToolbarClick(mouseX, mouseY);
        }
    }
    
    // Handle continuous drawing (mouse held + dragging)
    // NOTE: Only Pencil is continuous. Fill and Eraser are single-click operations
    if (isDrawing && input.isMouseButtonDown(SDL_BUTTON_LEFT) && !isPanning &&
        currentTool == Tool::PENCIL) {
        if (mouseX >= MAP_X && mouseX < MAP_X + MAP_W &&
            mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H) {
            // Only draw if not on sidebar
            if (!showLayerSidebar || mouseX >= SIDEBAR_X + SIDEBAR_W) {
                HandleViewportClick(mouseX, mouseY);
            }
        }
    }
    
    // Stop drawing when mouse released
    if (!input.isMouseButtonDown(SDL_BUTTON_LEFT)) {
        isDrawing = false;
        lastDrawnTileX = -1;
        lastDrawnTileY = -1;
    }
}

void MapEditor::Render(AestheticLayer& renderer, InputManager& input) {
    if (!isActive) return;
    
    // Clear screen
    renderer.Clear(1);  // Dark blue background
    
    // Render UI components (top to bottom)
    RenderTitleBar(renderer);
    RenderMapViewport(renderer);
    
    // Render layer sidebar if open (overlays on map)
    if (showLayerSidebar) {
        RenderLayerSidebar(renderer);
    }
    
    RenderToolbar(renderer);
    RenderSpritesheet(renderer);
    RenderStatusBar(renderer);
    
    // Render tooltip last (on top of everything)
    if (showTooltip) {
        int mouseX = input.getMouseX();
        int mouseY = input.getMouseY();
        
        // Create tooltip text
        std::string tooltipText = "X:" + std::to_string(tooltipTileX) + " Y:" + std::to_string(tooltipTileY);
        
        // Tooltip dimensions (8x8 font, so 8px per character)
        int tooltipW = tooltipText.length() * 8 + 4;  // 8px per char + 4px padding
        int tooltipH = 10;  // 8px font + 2px padding
        
        // Smart positioning - right of cursor by default, left if too close to right edge
        int tooltipX = mouseX + 12;  // Offset from cursor
        int tooltipY = mouseY + 12;
        
        // Adjust if going off right edge
        if (tooltipX + tooltipW > 256) {
            tooltipX = mouseX - tooltipW - 4;  // Show on left instead
        }
        
        // Adjust if going off bottom edge
        if (tooltipY + tooltipH > 256) {
            tooltipY = mouseY - tooltipH - 4;  // Show above instead
        }
        
        // Clamp to screen bounds
        if (tooltipX < 0) tooltipX = 0;
        if (tooltipY < 0) tooltipY = 0;
        if (tooltipX + tooltipW > 256) tooltipX = 256 - tooltipW;
        if (tooltipY + tooltipH > 256) tooltipY = 256 - tooltipH;
        
        // Draw tooltip background (white)
        renderer.RectFillRGB(tooltipX, tooltipY, tooltipW, tooltipH, 255, 255, 255);
        
        // Draw tooltip border (black)
        renderer.RectRGB(tooltipX, tooltipY, tooltipW, tooltipH, 0, 0, 0);
        
        // Draw tooltip text (black)
        renderer.PrintRGB(tooltipText.c_str(), tooltipX + 2, tooltipY + 1, 0, 0, 0);
    }
}

// ===== Rendering Methods =====

void MapEditor::RenderTitleBar(AestheticLayer& renderer) {
    // Light gray background bar (same as CodeEditor)
    renderer.RectFillRGB(0, 0, 256, TITLE_BAR_H,
                         SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    
    // Layer toggle button (left side)
    // Background for toggle
    SDL_Color toggleBg = showLayerSidebar ? SystemColors::GREEN : SystemColors::DARK_GRAY;
    renderer.RectFillRGB(TOGGLE_BTN_X, TOGGLE_BTN_Y, TOGGLE_BTN_SIZE, TOGGLE_BTN_SIZE,
                         toggleBg.r, toggleBg.g, toggleBg.b);
    renderer.RectRGB(TOGGLE_BTN_X, TOGGLE_BTN_Y, TOGGLE_BTN_SIZE, TOGGLE_BTN_SIZE,
                     SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // "L" text
    renderer.PrintRGB("L", TOGGLE_BTN_X + 2, TOGGLE_BTN_Y + 1,
                      SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Title text in black
    renderer.PrintRGB("MAP", 14, 1,
                      SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // Current layer info
    std::string layerText = layers[activeLayer].name;
    renderer.PrintRGB(layerText.c_str(), 160, 1,
                      SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    
    // Toast message - right after "MAP" text
    if (toastTimer > 0) {
        renderer.PrintRGB(toastMessage.c_str(), 35, 1,
                          SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
    }
}

void MapEditor::RenderMapViewport(AestheticLayer& renderer) {
    // White border around viewport
    renderer.RectRGB(MAP_BORDER_X, MAP_BORDER_Y, MAP_BORDER_W, MAP_BORDER_H,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // === CHECKBOARD BACKGROUND (8x8 to match tile size) ===
    int checkSize = 8;  // Same as TILE_SIZE
    for (int y = MAP_Y; y < MAP_Y + MAP_H; y += checkSize) {
        for (int x = MAP_X; x < MAP_X + MAP_W; x += checkSize) {
            bool isDark = ((x - MAP_X) / checkSize + (y - MAP_Y) / checkSize) % 2 == 0;
            if (isDark) {
                renderer.RectFillRGB(x, y, checkSize, checkSize, 102, 102, 102);  // Dark gray
            } else {
                renderer.RectFillRGB(x, y, checkSize, checkSize, 153, 153, 153);  // Light gray
            }
        }
    }
    
    // === MAP (complete 128x64 tiles, scaled by zoom) ===
    // Map position (starts at 4,4 by default, moves with cameraX/cameraY)
    int mapPixelWidth = MAP_WIDTH * TILE_SIZE * zoom;   // 128 * 8 * zoom
    int mapPixelHeight = MAP_HEIGHT * TILE_SIZE * zoom; // 64 * 8 * zoom
    int mapX = MAP_X + cameraX;  // Camera is the map offset
    int mapY = MAP_Y + cameraY;
    
    // White border around map
    renderer.RectRGB(mapX - 1, mapY - 1, mapPixelWidth + 2, mapPixelHeight + 2,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Black background for map
    renderer.RectFillRGB(mapX, mapY, mapPixelWidth, mapPixelHeight, 0, 0, 0);
    
    // === RENDER ALL TILES on the map (scaled by zoom) ===
    float tileSizeFloat = TILE_SIZE * zoom;  // 8 * zoom
    int tileSize = static_cast<int>(tileSizeFloat);
    
    for (int layer = 0; layer < LAYER_COUNT; layer++) {
        if (!layers[layer].visible) continue;
        
        for (int ty = 0; ty < MAP_HEIGHT; ty++) {
            for (int tx = 0; tx < MAP_WIDTH; tx++) {
                uint8_t tileId = GetTile(tx, ty, layer);
                // Tile 0 is special - it's "empty" and not rendered (PICO-8/TIC-80 convention)
                if (tileId == 0) continue;
                
                int screenX = mapX + tx * tileSizeFloat;
                int screenY = mapY + ty * tileSizeFloat;
                
                // Only draw if visible in viewport
                if (screenX + tileSize < MAP_X || screenX > MAP_X + MAP_W) continue;
                if (screenY + tileSize < MAP_Y || screenY > MAP_Y + MAP_H) continue;
                
                // Scale sprite to match tile size using DrawSpriteSection
                // Calculate sprite position in spritesheet (16 sprites per row)
                int spriteSheetX = (tileId % 16) * 8;
                int spriteSheetY = (tileId / 16) * 8;
                
                renderer.DrawSpriteSection(
                    spriteSheetX, spriteSheetY,  // Source position in spritesheet
                    8, 8,                         // Source size (always 8×8)
                    screenX, screenY,             // Destination position
                    tileSize, tileSize            // Destination size (scaled: 2, 4, 8, 16, 32)
                );
            }
        }
    }
    
    // === GRID OVERLAY (only visible at 1x and above) ===
    if (showGrid && zoom >= 1.0f) {
        // Draw grid lines every tileSizeFloat pixels
        SDL_Color gridColor = SystemColors::DARK_GRAY;
        
        // Vertical lines
        for (int tx = 0; tx <= MAP_WIDTH; tx++) {
            int lineX = mapX + tx * tileSizeFloat;
            // Only draw if visible in viewport
            if (lineX < MAP_X || lineX > MAP_X + MAP_W) continue;
            
            // Clamp line to map bounds and viewport
            int lineStartY = std::max(mapY, MAP_Y);
            int lineEndY = std::min(mapY + mapPixelHeight, MAP_Y + MAP_H);
            
            renderer.LineRGB(lineX, lineStartY, lineX, lineEndY,
                            gridColor.r, gridColor.g, gridColor.b);
        }
        
        // Horizontal lines
        for (int ty = 0; ty <= MAP_HEIGHT; ty++) {
            int lineY = mapY + ty * tileSizeFloat;
            // Only draw if visible in viewport
            if (lineY < MAP_Y || lineY > MAP_Y + MAP_H) continue;
            
            // Clamp line to map bounds and viewport
            int lineStartX = std::max(mapX, MAP_X);
            int lineEndX = std::min(mapX + mapPixelWidth, MAP_X + MAP_W);
            
            renderer.LineRGB(lineStartX, lineY, lineEndX, lineY,
                            gridColor.r, gridColor.g, gridColor.b);
        }
    }
    
    // === SCREEN SECTION GRID (always visible - shows game screen divisions) ===
    // Game screen is 128×64px (16×8 tiles in PICO-8)
    // Map is 1024×512px, so we have 8×8 = 64 screens
    int screenWidthTiles = 16;   // 128px / 8px per tile
    int screenHeightTiles = 8;   // 64px / 8px per tile
    float screenWidthPixels = screenWidthTiles * tileSizeFloat;
    float screenHeightPixels = screenHeightTiles * tileSizeFloat;
    
    SDL_Color sectionGridColor = {255, 0, 255};  // Magenta - very visible
    
    // Vertical section lines (every 16 tiles = 128px)
    for (int sx = 0; sx <= MAP_WIDTH; sx += screenWidthTiles) {
        int lineX = mapX + sx * tileSizeFloat;
        // Only draw if visible in viewport
        if (lineX < MAP_X || lineX > MAP_X + MAP_W) continue;
        
        // Clamp line to map bounds and viewport
        int lineStartY = std::max(mapY, MAP_Y);
        int lineEndY = std::min(mapY + mapPixelHeight, MAP_Y + MAP_H);
        
        renderer.LineRGB(lineX, lineStartY, lineX, lineEndY,
                        sectionGridColor.r, sectionGridColor.g, sectionGridColor.b);
    }
    
    // Horizontal section lines (every 8 tiles = 64px)
    for (int sy = 0; sy <= MAP_HEIGHT; sy += screenHeightTiles) {
        int lineY = mapY + sy * tileSizeFloat;
        // Only draw if visible in viewport
        if (lineY < MAP_Y || lineY > MAP_Y + MAP_H) continue;
        
        // Clamp line to map bounds and viewport
        int lineStartX = std::max(mapX, MAP_X);
        int lineEndX = std::min(mapX + mapPixelWidth, MAP_X + MAP_W);
        
        renderer.LineRGB(lineStartX, lineY, lineEndX, lineY,
                        sectionGridColor.r, sectionGridColor.g, sectionGridColor.b);
    }
}
//     // Draw grid overlay
//     if (showGrid) {
//         RenderGrid(renderer);
//     }
// }

void MapEditor::RenderGrid(AestheticLayer& renderer) {
    int startTileX = cameraX;
    int startTileY = cameraY;
    int endTileX = std::min(cameraX + (MAP_W / TILE_SIZE) + 1, MAP_WIDTH);
    int endTileY = std::min(cameraY + (MAP_H / TILE_SIZE) + 1, MAP_HEIGHT);
    
    // Draw vertical lines
    for (int tx = startTileX; tx <= endTileX; tx++) {
        int screenX = MAP_X + (tx - cameraX) * TILE_SIZE;
        if (screenX < MAP_X || screenX > MAP_X + MAP_W) continue;
        
        renderer.LineRGB(screenX, MAP_Y, screenX, MAP_Y + MAP_H,
                         SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
    }
    
    // Draw horizontal lines
    for (int ty = startTileY; ty <= endTileY; ty++) {
        int screenY = MAP_Y + (ty - cameraY) * TILE_SIZE;
        if (screenY < MAP_Y || screenY > MAP_Y + MAP_H) continue;
        
        renderer.LineRGB(MAP_X, screenY, MAP_X + MAP_W, screenY,
                         SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
    }
}

void MapEditor::RenderLayerSidebar(AestheticLayer& renderer) {
    // Background (dark gray)
    renderer.RectFillRGB(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_W, SIDEBAR_H,
                         SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
    
    // Border (white)
    renderer.RectRGB(SIDEBAR_X, SIDEBAR_Y, SIDEBAR_W, SIDEBAR_H,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Title "LAYERS"
    renderer.PrintRGB("LAYERS", SIDEBAR_X + 2, SIDEBAR_Y + SIDEBAR_TITLE_Y,
                      SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Render each layer
    int startY = SIDEBAR_Y + SIDEBAR_LIST_START_Y;  // Start below title
    
    for (int i = 0; i < LAYER_COUNT; i++) {
        int layerY = startY + i * LAYER_ITEM_H;
        
        // Background for this layer item
        SDL_Color itemBg = (i == activeLayer) ? SystemColors::GREEN : SystemColors::UI_CANVAS_BG;
        if (i == hoveredLayer) {
            itemBg = SystemColors::LAVENDER;  // Hover color
        }
        
        renderer.RectFillRGB(SIDEBAR_X + 2, layerY, SIDEBAR_W - 4, LAYER_ITEM_H - 2,
                             itemBg.r, itemBg.g, itemBg.b);
        
        // Layer number
        std::string layerNum = std::to_string(i);
        renderer.PrintRGB(layerNum.c_str(), SIDEBAR_X + LAYER_NUM_X, layerY + 4,
                          SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
        
        // Show/Hide text
        const char* visText = layers[i].visible ? "Show" : "Hide";
        renderer.PrintRGB(visText, SIDEBAR_X + LAYER_VIS_X, layerY + 4,
                          SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
        
        // Active indicator (arrow)
        if (i == activeLayer) {
            renderer.PrintRGB(">", SIDEBAR_X + LAYER_ARROW_X, layerY + 4,
                              SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
        }
    }
}

void MapEditor::RenderToolbar(AestheticLayer& renderer) {
    const int BUTTON_SIZE = 16;
    const int NUM_TOOLS = 5;
    
    const int toolIcons[] = { 0, 1, 20, 4, 16 };  // PENCIL, FILL, CLEAR, PICKER, GRID
    
    // Tool buttons at LEFT EDGE (x=0)
    int startX = 0;  // Start at x=0 (left edge)
    int startY = TOOLBAR_Y + 3;  // +3 (1px lower to avoid double border)
    
    // Render tool buttons
    for (int i = 0; i < NUM_TOOLS; i++) {
        int x = startX + (i * BUTTON_SIZE);
        int y = startY;
        
        renderer.RectFill(x, y, BUTTON_SIZE, BUTTON_SIZE, 0);
        
        SDL_Color bgColor = SystemColors::DARK_GRAY;
        bool isSelected = false;
        if (i == 0 && currentTool == Tool::PENCIL) isSelected = true;
        if (i == 1 && currentTool == Tool::FILL) isSelected = true;
        if (i == 2 && currentTool == Tool::ERASER) isSelected = true;
        if (i == 3 && currentTool == Tool::PICKER) isSelected = true;
        if (i == 4 && showGrid) isSelected = true;
        
        if (isSelected) bgColor = SystemColors::GREEN;
        
        renderer.RectFillRGB(x + 1, y + 1, BUTTON_SIZE - 2, BUTTON_SIZE - 2, bgColor.r, bgColor.g, bgColor.b);
        
        // 3D borders
        renderer.LineRGB(x + 1, y + 1, x + 1, y + 13, 
                         SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        renderer.LineRGB(x + 1, y + 1, x + 13, y + 1, 
                         SystemColors::LAVENDER.r, SystemColors::LAVENDER.g, SystemColors::LAVENDER.b);
        renderer.LineRGB(x + 14, y + 1, x + 14, y + 14, 
                         SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        renderer.LineRGB(x + 1, y + 14, x + 14, y + 14, 
                         SystemColors::DARK_BLUE.r, SystemColors::DARK_BLUE.g, SystemColors::DARK_BLUE.b);
        
        if (systemSprites) {
            systemSprites->DrawSprite(renderer, toolIcons[i], x + 4, y + 4, 1);
        }
    }
    
    // White border: LEFT, TOP, RIGHT only (NO BOTTOM to avoid double line)
    int totalToolWidth = NUM_TOOLS * BUTTON_SIZE;
    // Left border
    renderer.LineRGB(startX - 1, startY - 1, startX - 1, startY + BUTTON_SIZE,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    // Top border
    renderer.LineRGB(startX - 1, startY - 1, startX + totalToolWidth, startY - 1,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    // Right border
    renderer.LineRGB(startX + totalToolWidth, startY - 1, startX + totalToolWidth, startY + BUTTON_SIZE,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // === TAB SELECTOR (EXACTLY like Sprite Editor) ===
    // Draw 4 tab buttons styled EXACTLY like utility icons (16x16 with proper borders)
    // POSITIONED ON THE RIGHT EDGE OF SCREEN (ending at x=256)
    
    for (int i = 0; i < TAB_COUNT; i++) {
        int tabX = TAB_SELECTOR_X + (i * TAB_BUTTON_SIZE);  // Ends at x=256
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
    
    // White border: LEFT, TOP, RIGHT only (NO BOTTOM to avoid double line)
    int totalTabWidth = TAB_COUNT * TAB_BUTTON_SIZE;
    // Left border
    renderer.LineRGB(TAB_SELECTOR_X - 1, TAB_SELECTOR_Y - 1, TAB_SELECTOR_X - 1, TAB_SELECTOR_Y + TAB_BUTTON_SIZE,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    // Top border
    renderer.LineRGB(TAB_SELECTOR_X - 1, TAB_SELECTOR_Y - 1, TAB_SELECTOR_X + totalTabWidth, TAB_SELECTOR_Y - 1,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    // Right border
    renderer.LineRGB(TAB_SELECTOR_X + totalTabWidth, TAB_SELECTOR_Y - 1, TAB_SELECTOR_X + totalTabWidth, TAB_SELECTOR_Y + TAB_BUTTON_SIZE,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
}

void MapEditor::RenderSpritesheet(AestheticLayer& renderer) {
    // Spritesheet background
    renderer.RectFillRGB(SHEET_X, SHEET_Y, SHEET_W, SHEET_H,
                         SystemColors::UI_CANVAS_BG.r, SystemColors::UI_CANVAS_BG.g, SystemColors::UI_CANVAS_BG.b);
    
    // White border
    renderer.RectRGB(SHEET_X - 1, SHEET_Y - 1, SHEET_W + 2, SHEET_H + 2,
                     SystemColors::WHITE.r, SystemColors::WHITE.g, SystemColors::WHITE.b);
    
    // Draw sprites for current tab (16x4 grid)
    int baseSprite = currentTab * SPRITES_PER_TAB;  // 0, 64, 128, or 192
    
    for (int row = 0; row < SHEET_ROWS; row++) {
        for (int col = 0; col < SHEET_COLS; col++) {
            int localIndex = row * SHEET_COLS + col;
            uint8_t tileId = baseSprite + localIndex;
            
            int x = SHEET_X + col * SHEET_SPRITE_SIZE;
            int y = SHEET_Y + row * SHEET_SPRITE_SIZE;
            
            // Draw sprite centered in the 16x16 cell (sprite is 8x8)
            int centerX = x + (SHEET_SPRITE_SIZE - 8) / 2;
            int centerY = y + (SHEET_SPRITE_SIZE - 8) / 2;
            renderer.DrawSprite(tileId, centerX, centerY, 1, 1);
            
            // Highlight selected tile
            if (tileId == selectedTile) {
                renderer.RectRGB(x, y, SHEET_SPRITE_SIZE, SHEET_SPRITE_SIZE,
                                 SystemColors::YELLOW.r, SystemColors::YELLOW.g, SystemColors::YELLOW.b);
            }
        }
    }
}

void MapEditor::RenderStatusBar(AestheticLayer& renderer) {
    int y = 256 - STATUS_BAR_H;
    
    renderer.RectFillRGB(0, y, 256, STATUS_BAR_H,
                         SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    
    char statusText[128];
    sprintf(statusText, "(%d,%d) %s  Tile:%d  Tab:%d",
            cameraX, cameraY,
            (currentTool == Tool::PENCIL ? "PEN" :
             currentTool == Tool::FILL ? "FILL" :
             currentTool == Tool::ERASER ? "ERASE" : "PICK"),
            selectedTile, currentTab);
    
    renderer.PrintRGB(statusText, 2, y + 1,
                      SystemColors::BLACK.r, SystemColors::BLACK.g, SystemColors::BLACK.b);
}

// ===== Input Handling =====

void MapEditor::HandleViewportClick(int mouseX, int mouseY) {
    // Convert screen to tile coordinates
    int tileX = ScreenToTileX(mouseX);
    int tileY = ScreenToTileY(mouseY);
    
    if (!IsValidTileCoord(tileX, tileY)) return;
    
    // Apply current tool
    switch (currentTool) {
        case Tool::PENCIL:
            // Only save on first click, not during continuous drawing
            {
                bool shouldSaveUndo = !isDrawing || (lastDrawnTileX == -1 && lastDrawnTileY == -1);
                UsePencil(tileX, tileY);
                if (shouldSaveUndo) SaveUndoState("Paint");
            }
            break;
            
        case Tool::FILL:
            {
                uint8_t targetTile = GetTile(tileX, tileY, activeLayer);
                UseFill(tileX, tileY, targetTile, selectedTile);
                SaveUndoState("Fill");
            }
            break;
            
        case Tool::ERASER:
            // Only save on first click, not during continuous drawing
            {
                bool shouldSaveUndo = !isDrawing || (lastDrawnTileX == -1 && lastDrawnTileY == -1);
                UseEraser(tileX, tileY);
                if (shouldSaveUndo) SaveUndoState("Erase");
            }
            break;
            
        case Tool::PICKER:
            UsePicker(tileX, tileY);
            break;
    }
}

void MapEditor::HandlePickerClick(int mouseX, int mouseY) {
    // Convert to local spritesheet coordinates
    int col = (mouseX - SHEET_X) / SHEET_SPRITE_SIZE;
    int row = (mouseY - SHEET_Y) / SHEET_SPRITE_SIZE;
    
    if (col >= 0 && col < SHEET_COLS && row >= 0 && row < SHEET_ROWS) {
        int localIndex = row * SHEET_COLS + col;
        selectedTile = (currentTab * SPRITES_PER_TAB) + localIndex;
        std::cout << "[MapEditor] Selected tile: " << (int)selectedTile << std::endl;
    }
}

void MapEditor::HandleToolbarClick(int mouseX, int mouseY) {
    const int BUTTON_SIZE = 16;
    const int NUM_TOOLS = 5;
    const int startX = 0;  // Left edge
    const int startY = TOOLBAR_Y + 3;  // Match rendering position
    
    // Check for tool button clicks
    if (mouseY >= startY && mouseY < startY + BUTTON_SIZE) {
        if (mouseX >= startX && mouseX < startX + (NUM_TOOLS * BUTTON_SIZE)) {
            int buttonIndex = (mouseX - startX) / BUTTON_SIZE;
            
            if (buttonIndex >= 0 && buttonIndex < NUM_TOOLS) {
                switch (buttonIndex) {
                    case 0: currentTool = Tool::PENCIL; break;
                    case 1: currentTool = Tool::FILL; break;
                    case 2: currentTool = Tool::ERASER; break;
                    case 3: currentTool = Tool::PICKER; break;
                    case 4: showGrid = !showGrid; break;
                }
            }
            return;
        }
    }
    
    // Check for tab button clicks (on the RIGHT side)
    int tabY = TAB_SELECTOR_Y;
    
    if (mouseY >= tabY && mouseY < tabY + TAB_BUTTON_SIZE) {
        if (mouseX >= TAB_SELECTOR_X && mouseX < TAB_SELECTOR_X + (TAB_COUNT * TAB_BUTTON_SIZE)) {
            int tabIndex = (mouseX - TAB_SELECTOR_X) / TAB_BUTTON_SIZE;
            if (tabIndex >= 0 && tabIndex < TAB_COUNT) {
                currentTab = tabIndex;
                std::cout << "[MapEditor] Switched to tab: " << currentTab << std::endl;
            }
        }
    }
}

void MapEditor::HandleLayerClick(int mouseX, int mouseY) {
    // TODO: Implement layer button clicks
}

void MapEditor::HandleKeyboard(InputManager& input) {
    // Layer selection (1-8 keys)
    for (int i = 0; i < LAYER_COUNT; i++) {
        if (input.isKeyPressed(static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i))) {
            SetActiveLayer(i);
        }
    }
    
    // Camera movement (Arrow keys)
    if (input.isKeyDown(SDL_SCANCODE_LEFT)) cameraX = std::max(0, cameraX - 1);
    if (input.isKeyDown(SDL_SCANCODE_RIGHT)) cameraX = std::min(MAP_WIDTH - (MAP_W / TILE_SIZE), cameraX + 1);
    if (input.isKeyDown(SDL_SCANCODE_UP)) cameraY = std::max(0, cameraY - 1);
    if (input.isKeyDown(SDL_SCANCODE_DOWN)) cameraY = std::min(MAP_HEIGHT - (MAP_H / TILE_SIZE), cameraY + 1);
    
    // Toggle grid (G key)
    if (input.isKeyPressed(SDL_SCANCODE_G)) {
        showGrid = !showGrid;
    }
    
    // Toggle layer sidebar (L key)
    if (input.isKeyPressed(SDL_SCANCODE_L)) {
        showLayerSidebar = !showLayerSidebar;
        std::cout << "[MapEditor] Layer sidebar: " << (showLayerSidebar ? "open" : "closed") << std::endl;
    }
    
    // Save (Ctrl+S)
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_S)) {
        if (!mapPath.empty()) {
            SaveToJSON(mapPath);
        }
    }
    
    // Undo (Ctrl+Z)
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_Z)) {
        Undo();
    }
    
    // Redo (Ctrl+Y)
    if (input.isCtrlDown() && input.isKeyPressed(SDL_SCANCODE_Y)) {
        Redo();
    }
}

// ===== Tool Implementations =====

void MapEditor::UsePencil(int tileX, int tileY) {
    // Skip if we just painted this tile (prevents double-painting during drag)
    if (tileX == lastDrawnTileX && tileY == lastDrawnTileY) {
        return;
    }
    
    SetTile(tileX, tileY, activeLayer, selectedTile);
    lastDrawnTileX = tileX;
    lastDrawnTileY = tileY;
}

void MapEditor::UseFill(int tileX, int tileY, uint8_t targetTile, uint8_t replacementTile) {
    if (targetTile == replacementTile) return;  // Nothing to do
    FloodFillRecursive(tileX, tileY, activeLayer, targetTile, replacementTile);
}

void MapEditor::UseEraser(int tileX, int tileY) {
    SetTile(tileX, tileY, activeLayer, 0);  // 0 = transparent tile
}

void MapEditor::UsePicker(int tileX, int tileY) {
    selectedTile = GetTile(tileX, tileY, activeLayer);
    std::cout << "[MapEditor] Picked tile: " << (int)selectedTile << std::endl;
}

void MapEditor::FloodFillRecursive(int x, int y, int layer, uint8_t targetTile, uint8_t replacementTile) {
    if (!IsValidTileCoord(x, y)) return;
    if (GetTile(x, y, layer) != targetTile) return;
    
    // Use iterative flood fill with queue to avoid stack overflow
    struct Point { int x, y; };
    std::vector<Point> queue;
    queue.push_back({x, y});
    
    int fillCount = 0;
    const int MAX_FILL = 10000;  // Safety limit
    
    while (!queue.empty() && fillCount < MAX_FILL) {
        Point p = queue.back();
        queue.pop_back();
        
        // Skip if out of bounds or already filled
        if (!IsValidTileCoord(p.x, p.y)) continue;
        if (GetTile(p.x, p.y, layer) != targetTile) continue;
        
        // Fill this tile
        SetTile(p.x, p.y, layer, replacementTile);
        fillCount++;
        
        // Add neighbors to queue (4-way)
        if (p.x + 1 < MAP_WIDTH) queue.push_back({p.x + 1, p.y});
        if (p.x - 1 >= 0) queue.push_back({p.x - 1, p.y});
        if (p.y + 1 < MAP_HEIGHT) queue.push_back({p.x, p.y + 1});
        if (p.y - 1 >= 0) queue.push_back({p.x, p.y - 1});
    }
    
    if (fillCount >= MAX_FILL) {
        std::cout << "[MapEditor] Fill limit reached (safety)" << std::endl;
    }
}

// ===== Map Manipulation =====

void MapEditor::SetTile(int x, int y, int layer, uint8_t tileId) {
    if (!IsValidTileCoord(x, y) || layer < 0 || layer >= LAYER_COUNT) return;
    
    int index = y * MAP_WIDTH + x;
    layers[layer].data[index] = tileId;
}

uint8_t MapEditor::GetTile(int x, int y, int layer) const {
    if (!IsValidTileCoord(x, y) || layer < 0 || layer >= LAYER_COUNT) return 0;
    
    int index = y * MAP_WIDTH + x;
    return layers[layer].data[index];
}

void MapEditor::ClearLayer(int layer) {
    if (layer < 0 || layer >= LAYER_COUNT) return;
    std::fill(layers[layer].data.begin(), layers[layer].data.end(), 0);
}

void MapEditor::ClearAllLayers() {
    for (auto& layer : layers) {
        std::fill(layer.data.begin(), layer.data.end(), 0);
    }
}

// ===== Layer Management =====

void MapEditor::SetActiveLayer(int layer) {
    if (layer >= 0 && layer < LAYER_COUNT) {
        activeLayer = layer;
        std::cout << "[MapEditor] Active layer: " << layer << " (" << layers[layer].name << ")" << std::endl;
    }
}

void MapEditor::ToggleLayerVisibility(int layer) {
    if (layer >= 0 && layer < LAYER_COUNT) {
        layers[layer].visible = !layers[layer].visible;
    }
}

bool MapEditor::IsLayerVisible(int layer) const {
    if (layer >= 0 && layer < LAYER_COUNT) {
        return layers[layer].visible;
    }
    return false;
}

void MapEditor::SetLayerName(int layer, const std::string& name) {
    if (layer >= 0 && layer < LAYER_COUNT) {
        layers[layer].name = name;
    }
}

std::string MapEditor::GetLayerName(int layer) const {
    if (layer >= 0 && layer < LAYER_COUNT) {
        return layers[layer].name;
    }
    return "";
}

// ===== File Operations =====

bool MapEditor::SaveToJSON(const std::string& path) {
    try {
        json j;
        j["version"] = "1.0";
        j["width"] = MAP_WIDTH;
        j["height"] = MAP_HEIGHT;
        j["tileSize"] = TILE_SIZE;
        
        // Save layers
        json layersArray = json::array();
        for (int i = 0; i < LAYER_COUNT; i++) {
            json layerObj;
            layerObj["id"] = i;
            layerObj["name"] = layers[i].name;
            layerObj["visible"] = layers[i].visible;
            layerObj["opacity"] = layers[i].opacity;
            layerObj["data"] = layers[i].data;
            
            layersArray.push_back(layerObj);
        }
        j["layers"] = layersArray;
        
        // Write to file
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "[MapEditor] Failed to open file for writing: " << path << std::endl;
            return false;
        }
        
        file << j.dump(2);  // Pretty print with 2-space indent
        file.close();
        
        std::cout << "[MapEditor] Saved map to: " << path << std::endl;
        ShowToast("Map saved!");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[MapEditor] Error saving map: " << e.what() << std::endl;
        ShowToast("Save failed!");
        return false;
    }
}

bool MapEditor::LoadFromJSON(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;  // File doesn't exist yet
        }
        
        json j;
        file >> j;
        file.close();
        
        // Validate version
        if (!j.contains("version") || !j.contains("layers")) {
            std::cerr << "[MapEditor] Invalid map file format" << std::endl;
            return false;
        }
        
        // Load layers
        for (const auto& layerObj : j["layers"]) {
            int id = layerObj["id"];
            if (id >= 0 && id < LAYER_COUNT) {
                layers[id].name = layerObj["name"];
                layers[id].visible = layerObj["visible"];
                layers[id].opacity = layerObj["opacity"];
                layers[id].data = layerObj["data"].get<std::vector<uint8_t>>();
            }
        }
        
        std::cout << "[MapEditor] Loaded map from: " << path << std::endl;
        ShowToast("Map loaded!");
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[MapEditor] Error loading map: " << e.what() << std::endl;
        ShowToast("Load failed!");
        return false;
    }
}

// ===== Helper Methods =====

bool MapEditor::IsValidTileCoord(int x, int y) const {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

int MapEditor::ScreenToTileX(int screenX) const {
    // Map position
    int mapX = MAP_X + cameraX;
    float tileSize = TILE_SIZE * zoom;
    // Convert screen to tile
    return (screenX - mapX) / tileSize;
}

int MapEditor::ScreenToTileY(int screenY) const {
    // Map position
    int mapY = MAP_Y + cameraY;
    float tileSize = TILE_SIZE * zoom;
    // Convert screen to tile
    return (screenY - mapY) / tileSize;
}

int MapEditor::TileToScreenX(int tileX) const {
    // Map position
    int mapX = MAP_X + cameraX;
    float tileSize = TILE_SIZE * zoom;
    return mapX + tileX * tileSize;
}

int MapEditor::TileToScreenY(int tileY) const {
    // Map position
    int mapY = MAP_Y + cameraY;
    float tileSize = TILE_SIZE * zoom;
    return mapY + tileY * tileSize;
}

// === Map Drawing Helper Methods ===

int MapEditor::GetMapPixelWidth() const {
    return MAP_WIDTH * TILE_SIZE * zoom;
}

int MapEditor::GetMapPixelHeight() const {
    return MAP_HEIGHT * TILE_SIZE * zoom;
}

int MapEditor::GetMapDrawX() const {
    return MAP_X + (MAP_W - GetMapPixelWidth()) / 2 - cameraX;
}

int MapEditor::GetMapDrawY() const {
    return MAP_Y + (MAP_H - GetMapPixelHeight()) / 2 - cameraY;
}

float MapEditor::GetTileSize() const {
    return TILE_SIZE * zoom;
}

void MapEditor::ShowToast(const std::string& message) {
    toastMessage = message;
    toastTimer = TOAST_DURATION;
    std::cout << "[MapEditor] Toast: " << message << std::endl;
}

void MapEditor::Log(const std::string& message) {
    std::cout << "[MapEditor] " << message << std::endl;
}

// ===== Cursor Management =====

void MapEditor::InitializeCursors() {
    // Create system cursors using SDL built-in cursors
    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    crossCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    
    // Set default cursor
    SetCursor(CursorType::ARROW);
    
    std::cout << "[MapEditor] Cursors initialized" << std::endl;
}

void MapEditor::CleanupCursors() {
    if (arrowCursor) SDL_FreeCursor(arrowCursor);
    if (handCursor) SDL_FreeCursor(handCursor);
    if (crossCursor) SDL_FreeCursor(crossCursor);
    
    arrowCursor = nullptr;
    handCursor = nullptr;
    crossCursor = nullptr;
}

void MapEditor::UpdateCursor(int mouseX, int mouseY) {
    CursorType newCursor = CursorType::ARROW;  // Default
    
    // Determine which cursor to show based on UI area
    // Map viewport - Crosshair
    if (mouseX >= MAP_X && mouseX < MAP_X + MAP_W &&
        mouseY >= MAP_Y && mouseY < MAP_Y + MAP_H) {
        if (!showLayerSidebar || mouseX >= SIDEBAR_X + SIDEBAR_W) {
            newCursor = CursorType::CROSSHAIR;
        } else {
            newCursor = CursorType::HAND;  // Sidebar area
        }
    }
    // Toolbar - Hand
    else if (mouseY >= TOOLBAR_Y && mouseY < TOOLBAR_Y + TOOLBAR_H) {
        newCursor = CursorType::HAND;
    }
    // Spritesheet - Hand
    else if (mouseX >= SHEET_X && mouseX < SHEET_X + SHEET_W &&
             mouseY >= SHEET_Y && mouseY < SHEET_Y + SHEET_H) {
        newCursor = CursorType::HAND;
    }
    // Layer sidebar - Hand
    else if (showLayerSidebar && mouseX >= SIDEBAR_X && mouseX < SIDEBAR_X + SIDEBAR_W &&
             mouseY >= SIDEBAR_Y && mouseY < SIDEBAR_Y + SIDEBAR_H) {
        newCursor = CursorType::HAND;
    }
    // Title bar - Hand (for layer toggle button)
    else if (mouseY >= 0 && mouseY < TITLE_BAR_H) {
        newCursor = CursorType::HAND;
    }
    
    // Only change cursor if it's different (performance optimization)
    if (newCursor != currentCursor) {
        SetCursor(newCursor);
    }
}

void MapEditor::SetCursor(CursorType type) {
    currentCursor = type;
    
    switch (type) {
        case CursorType::ARROW:
            if (arrowCursor) SDL_SetCursor(arrowCursor);
            break;
        case CursorType::HAND:
            if (handCursor) SDL_SetCursor(handCursor);
            break;
        case CursorType::CROSSHAIR:
            if (crossCursor) SDL_SetCursor(crossCursor);
            break;
    }
}
