#ifndef SYSTEM_COLORS_H
#define SYSTEM_COLORS_H

#include <SDL.h>

/**
 * @brief System-wide UI color constants
 * 
 * These RGB colors are FIXED and never change with palette imports.
 * All UI elements (text, borders, backgrounds, icons) should use these colors.
 * 
 * This ensures that importing custom palettes in editors (Sprite Editor, Map Editor, etc.)
 * does NOT affect the application's UI appearance.
 */
namespace SystemColors {
    
    // === Core Colors (PICO-8 inspired) ===
    constexpr SDL_Color BLACK       = {0, 0, 0, 255};
    constexpr SDL_Color DARK_BLUE   = {29, 43, 83, 255};
    constexpr SDL_Color DARK_PURPLE = {126, 37, 83, 255};
    constexpr SDL_Color DARK_GREEN  = {0, 135, 81, 255};
    constexpr SDL_Color BROWN       = {171, 82, 54, 255};
    constexpr SDL_Color DARK_GRAY   = {95, 87, 79, 255};
    constexpr SDL_Color LIGHT_GRAY  = {194, 195, 199, 255};
    constexpr SDL_Color WHITE       = {255, 241, 232, 255};
    constexpr SDL_Color RED         = {255, 0, 77, 255};
    constexpr SDL_Color ORANGE      = {255, 163, 0, 255};
    constexpr SDL_Color YELLOW      = {255, 236, 39, 255};
    constexpr SDL_Color GREEN       = {0, 228, 54, 255};
    constexpr SDL_Color BLUE        = {41, 173, 255, 255};
    constexpr SDL_Color LAVENDER    = {131, 118, 156, 255};
    constexpr SDL_Color PINK        = {255, 119, 168, 255};
    constexpr SDL_Color PEACH       = {255, 204, 170, 255};
    
    // === UI-Specific Aliases (for clarity) ===
    
    // Backgrounds
    constexpr SDL_Color UI_BACKGROUND      = DARK_BLUE;   // Main UI background
    constexpr SDL_Color UI_CANVAS_BG       = BLACK;       // Canvas/editor background
    constexpr SDL_Color UI_TOOLBAR_BG      = DARK_BLUE;   // Toolbar background
    constexpr SDL_Color UI_STATUSBAR_BG    = DARK_BLUE;   // Status bar background
    
    // Text
    constexpr SDL_Color UI_TEXT_PRIMARY    = WHITE;       // Main text color
    constexpr SDL_Color UI_TEXT_SECONDARY  = LIGHT_GRAY;  // Secondary text
    constexpr SDL_Color UI_TEXT_DISABLED   = DARK_GRAY;   // Disabled text
    
    // Borders & Lines
    constexpr SDL_Color UI_BORDER_LIGHT    = WHITE;       // Light borders
    constexpr SDL_Color UI_BORDER_MEDIUM   = LIGHT_GRAY;  // Medium borders
    constexpr SDL_Color UI_BORDER_DARK     = DARK_GRAY;   // Dark borders
    constexpr SDL_Color UI_GRID            = DARK_GRAY;   // Grid lines
    
    // Interactive Elements
    constexpr SDL_Color UI_SELECTED        = BLUE;        // Selected items
    constexpr SDL_Color UI_HOVER           = LAVENDER;    // Hover state
    constexpr SDL_Color UI_ACTIVE          = GREEN;       // Active/pressed state
    constexpr SDL_Color UI_HIGHLIGHT       = YELLOW;      // Highlights
    constexpr SDL_Color UI_ERROR           = RED;         // Error states
    constexpr SDL_Color UI_WARNING         = ORANGE;      // Warning states
    
    // Icons & Sprites (used by SystemSprites)
    constexpr SDL_Color ICON_FOREGROUND    = WHITE;
    constexpr SDL_Color ICON_ACCENT        = BLUE;
    constexpr SDL_Color ICON_DISABLED      = DARK_GRAY;
    
    // === Helper: Get color from PICO-8 index (for compatibility) ===
    // Returns the fixed RGB color for a given palette index (0-15)
    // This allows legacy code using indices to work without palette dependency
    inline SDL_Color GetFixedColor(int index) {
        constexpr SDL_Color FIXED_PALETTE[16] = {
            BLACK, DARK_BLUE, DARK_PURPLE, DARK_GREEN,
            BROWN, DARK_GRAY, LIGHT_GRAY, WHITE,
            RED, ORANGE, YELLOW, GREEN,
            BLUE, LAVENDER, PINK, PEACH
        };
        
        if (index >= 0 && index < 16) {
            return FIXED_PALETTE[index];
        }
        return BLACK;  // Default to black for out-of-range
    }
    
    // === Extended Colors (TIC-80 style, for future use) ===
    constexpr SDL_Color VERY_DARK_BLUE  = {26, 28, 44, 255};
    constexpr SDL_Color PURPLE_2        = {93, 39, 93, 255};
    constexpr SDL_Color RED_PURPLE      = {177, 62, 83, 255};
    constexpr SDL_Color LIGHT_ORANGE    = {239, 125, 87, 255};
    constexpr SDL_Color LIGHT_YELLOW    = {255, 205, 117, 255};
    constexpr SDL_Color LIGHT_GREEN     = {167, 240, 112, 255};
    constexpr SDL_Color MEDIUM_GREEN    = {56, 183, 100, 255};
    constexpr SDL_Color TEAL            = {37, 113, 121, 255};
    constexpr SDL_Color NAVY            = {41, 54, 111, 255};
    constexpr SDL_Color ROYAL_BLUE      = {59, 93, 201, 255};
    constexpr SDL_Color SKY_BLUE        = {65, 166, 246, 255};
    constexpr SDL_Color CYAN            = {115, 239, 247, 255};
    constexpr SDL_Color OFF_WHITE       = {244, 244, 244, 255};
    constexpr SDL_Color BLUE_GRAY_LIGHT = {148, 176, 194, 255};
    constexpr SDL_Color BLUE_GRAY       = {86, 108, 134, 255};
    constexpr SDL_Color BLUE_GRAY_DARK  = {51, 60, 87, 255};
}

#endif // SYSTEM_COLORS_H
