#ifndef UISYSTEM_H
#define UISYSTEM_H

#include <string>
#include <vector>

// Forward declarations
class AestheticLayer;
class InputManager;

/**
 * @brief UISystem - Custom pixel-perfect retro UI system
 * 
 * Renders UI using AestheticLayer primitives (rectfill, print, pset, line).
 * 100% custom, no external libraries. PICO-8/TIC-80 inspired aesthetic.
 * 
 * Features:
 * - Panel rendering (windows with title bars)
 * - Text rendering (multi-line, word wrap)
 * - Input handling (keyboard, mouse)
 * - Retro color scheme
 * 
 * Phase 2.0.1 - Foundation for Code Editor, Sprite Editor, Map Editor
 */
class UISystem {
public:
    UISystem();
    ~UISystem();

    // ============================================
    // PANEL RENDERING
    // ============================================
    
    /**
     * @brief Render a panel (window) with title bar
     * @param layer Rendering layer
     * @param x X position
     * @param y Y position
     * @param w Width
     * @param h Height
     * @param title Title text
     * @param focused Is this panel focused?
     */
    void RenderPanel(AestheticLayer& layer, int x, int y, int w, int h, 
                     const std::string& title, bool focused = true);

    /**
     * @brief Render a simple box (no title)
     */
    void RenderBox(AestheticLayer& layer, int x, int y, int w, int h, int color);

    /**
     * @brief Render a separator line
     */
    void RenderSeparator(AestheticLayer& layer, int x, int y, int w, int color);

    // ============================================
    // TEXT RENDERING
    // ============================================
    
    /**
     * @brief Render multi-line text with word wrap
     * @param layer Rendering layer
     * @param text Text to render
     * @param x X position
     * @param y Y position
     * @param maxWidth Maximum width before wrapping
     * @param color Text color
     * @return Number of lines rendered
     */
    int RenderText(AestheticLayer& layer, const std::string& text, 
                   int x, int y, int maxWidth, int color);

    /**
     * @brief Render a single line of text (no wrapping)
     */
    void RenderLine(AestheticLayer& layer, const std::string& text, 
                    int x, int y, int color);

    /**
     * @brief Render text centered in a box
     */
    void RenderCenteredText(AestheticLayer& layer, const std::string& text,
                            int x, int y, int w, int h, int color);

    // ============================================
    // BUTTON RENDERING
    // ============================================
    
    /**
     * @brief Render a button
     * @return true if button is hovered
     */
    bool RenderButton(AestheticLayer& layer, const std::string& text,
                      int x, int y, int w, int h, bool pressed = false);

    // ============================================
    // INPUT UTILITIES
    // ============================================
    
    /**
     * @brief Check if mouse is inside a rectangle
     */
    bool IsMouseInRect(int mx, int my, int x, int y, int w, int h);

    /**
     * @brief Get character width (4 pixels in PICO-8 style font)
     */
    int GetCharWidth() const { return 4; }

    /**
     * @brief Get character height (6 pixels in PICO-8 style font)
     */
    int GetCharHeight() const { return 6; }

    /**
     * @brief Calculate text width in pixels
     */
    int GetTextWidth(const std::string& text) const;

private:
    // Helper: Word wrap text
    std::vector<std::string> WrapText(const std::string& text, int maxWidth);
};

#endif // UISYSTEM_H
