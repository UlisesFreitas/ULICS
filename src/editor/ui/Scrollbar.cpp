#include "editor/ui/Scrollbar.h"
#include "input/InputManager.h"
#include "rendering/AestheticLayer.h"
#include <algorithm>

namespace UliCS {

Scrollbar::Scrollbar()
    : posX(0)
    , posY(0)
    , scrollbarWidth(4)
    , scrollbarHeight(100)
    , totalContentLines(0)
    , visibleLines(20)
    , currentScrollY(0)
    , isDragging(false)
    , dragOffset(0) {
}

Scrollbar::~Scrollbar() {
}

void Scrollbar::SetDimensions(int x, int y, int width, int height) {
    posX = x;
    posY = y;
    scrollbarWidth = width;
    scrollbarHeight = height;
}

void Scrollbar::SetContentSize(int totalLines) {
    totalContentLines = totalLines;
}

void Scrollbar::SetVisibleSize(int lines) {
    visibleLines = lines;
}

void Scrollbar::SetScrollPosition(int scrollY) {
    currentScrollY = scrollY;
    
    // Clamp to valid range
    int maxScroll = totalContentLines - visibleLines;
    if (maxScroll < 0) maxScroll = 0;
    
    if (currentScrollY < 0) currentScrollY = 0;
    if (currentScrollY > maxScroll) currentScrollY = maxScroll;
}

int Scrollbar::GetScrollPosition() const {
    return currentScrollY;
}

bool Scrollbar::HandleInput(InputManager& input) {
    if (!ShouldRender()) {
        return false;
    }
    
    int mouseX = input.getMouseX();
    int mouseY = input.getMouseY();
    bool mouseDown = input.isMouseButtonDown(1); // Left button
    bool mousePressed = input.isMouseButtonPressed(1);
    
    // Check if mouse is over scrollbar area
    bool mouseOverScrollbar = (mouseX >= posX && mouseX < posX + scrollbarWidth &&
                               mouseY >= posY && mouseY < posY + scrollbarHeight);
    
    // Start dragging if mouse pressed on thumb
    if (mousePressed && mouseOverScrollbar) {
        if (IsMouseOverThumb(mouseX, mouseY)) {
            isDragging = true;
            
            // Calculate offset from top of thumb
            int thumbY, thumbHeight;
            CalculateThumbBounds(thumbY, thumbHeight);
            dragOffset = mouseY - thumbY;
            return true;
        }
    }
    
    // Continue dragging
    if (isDragging && mouseDown) {
        int newScrollY = MouseYToScrollPosition(mouseY - dragOffset);
        SetScrollPosition(newScrollY);
        return true;
    }
    
    // End dragging
    if (isDragging && !mouseDown) {
        isDragging = false;
        return true;
    }
    
    return false;
}

void Scrollbar::Render(AestheticLayer& layer) {
    if (!ShouldRender()) {
        return;
    }
    
    // Draw track (dark grey background)
    const int COLOR_TRACK = 5;   // UISystem::COLOR_DARK_GRAY
    const int COLOR_THUMB = 7;   // UISystem::COLOR_WHITE
    
    layer.RectFill(posX, posY, scrollbarWidth, scrollbarHeight, COLOR_TRACK);
    
    // Calculate and draw thumb
    int thumbY, thumbHeight;
    CalculateThumbBounds(thumbY, thumbHeight);
    
    // Draw thumb
    layer.RectFill(posX, thumbY, scrollbarWidth, thumbHeight, COLOR_THUMB);
}

bool Scrollbar::ShouldRender() const {
    return totalContentLines > visibleLines;
}

void Scrollbar::ResetDrag() {
    isDragging = false;
    dragOffset = 0;
}

// Private helper methods

void Scrollbar::CalculateThumbBounds(int& outThumbY, int& outThumbHeight) const {
    if (totalContentLines <= visibleLines) {
        outThumbY = posY;
        outThumbHeight = scrollbarHeight;
        return;
    }
    
    // Thumb height proportional to visible/total ratio
    float visibleRatio = static_cast<float>(visibleLines) / static_cast<float>(totalContentLines);
    outThumbHeight = static_cast<int>(scrollbarHeight * visibleRatio);
    
    // Minimum thumb height for usability
    if (outThumbHeight < 20) {
        outThumbHeight = 20;
    }
    
    // Thumb position based on scroll percentage
    int scrollableLines = totalContentLines - visibleLines;
    float scrollPercent = 0.0f;
    if (scrollableLines > 0) {
        scrollPercent = static_cast<float>(currentScrollY) / static_cast<float>(scrollableLines);
    }
    
    int scrollablePixels = scrollbarHeight - outThumbHeight;
    outThumbY = posY + static_cast<int>(scrollablePixels * scrollPercent);
}

bool Scrollbar::IsMouseOverThumb(int mouseX, int mouseY) const {
    if (mouseX < posX || mouseX >= posX + scrollbarWidth) {
        return false;
    }
    
    int thumbY, thumbHeight;
    CalculateThumbBounds(thumbY, thumbHeight);
    
    return mouseY >= thumbY && mouseY < thumbY + thumbHeight;
}

int Scrollbar::MouseYToScrollPosition(int mouseY) const {
    int thumbHeight;
    int dummy;
    CalculateThumbBounds(dummy, thumbHeight);
    
    // Calculate scrollable area
    int scrollablePixels = scrollbarHeight - thumbHeight;
    if (scrollablePixels <= 0) {
        return 0;
    }
    
    // Mouse position relative to scrollbar top
    int relativeY = mouseY - posY;
    
    // Clamp to scrollable area
    if (relativeY < 0) relativeY = 0;
    if (relativeY > scrollablePixels) relativeY = scrollablePixels;
    
    // Convert pixel position to line number
    float scrollPercent = static_cast<float>(relativeY) / static_cast<float>(scrollablePixels);
    int scrollableLines = totalContentLines - visibleLines;
    
    return static_cast<int>(scrollPercent * scrollableLines);
}

} // namespace UliCS
