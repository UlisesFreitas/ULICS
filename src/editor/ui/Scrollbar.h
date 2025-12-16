#pragma once

// Forward declarations
class InputManager;
class AestheticLayer;

namespace UliCS {

/**
 * @brief Manages vertical scrollbar UI and input
 * 
 * Handles scrollbar rendering, mouse interaction (clicking, dragging),
 * and position calculation for content scrolling.
 */
class Scrollbar {
public:
    Scrollbar();
    ~Scrollbar();
    
    /**
     * @brief Initialize scrollbar dimensions
     * @param x X position of scrollbar
     * @param y Y position of scrollbar (top)
     * @param width Width of scrollbar
     * @param height Height of scrollbar area
     */
    void SetDimensions(int x, int y, int width, int height);
    
    /**
     * @brief Set content size (total lines available)
     */
    void SetContentSize(int totalLines);
    
    /**
     * @brief Set visible area size (lines visible at once)
     */
    void SetVisibleSize(int visibleLines);
    
    /**
     * @brief Set current scroll position
     * @param scrollY Current scroll position (line index)
     */
    void SetScrollPosition(int scrollY);
    
    /**
     * @brief Get current scroll position
     */
    int GetScrollPosition() const;
    
    /**
     * @brief Handle mouse input for scrollbar
     * @param input Input manager
     * @return true if scrollbar consumed the input
     */
    bool HandleInput(InputManager& input);
    
    /**
     * @brief Render the scrollbar
     */
    void Render(AestheticLayer& layer);
    
    /**
     * @brief Check if scrollbar should be visible
     * @return true if content exceeds visible area
     */
    bool ShouldRender() const;
    
    /**
     * @brief Reset dragging state (call when losing focus)
     */
    void ResetDrag();

private:
    // Dimensions
    int posX;
    int posY;
    int scrollbarWidth;
    int scrollbarHeight;
    
    // Content info
    int totalContentLines;
    int visibleLines;
    int currentScrollY;
    
    // Interaction state
    bool isDragging;
    int dragOffset;  // Offset from thumb top when drag started
    
    // Helper methods
    void CalculateThumbBounds(int& outThumbY, int& outThumbHeight) const;
    bool IsMouseOverThumb(int mouseX, int mouseY) const;
    int MouseYToScrollPosition(int mouseY) const;
};

} // namespace UliCS
