#include "ui/UISystem.h"
#include "rendering/AestheticLayer.h"
#include <sstream>
#include <algorithm>

UISystem::UISystem() {
}

UISystem::~UISystem() {
}

// ============================================
// PANEL RENDERING
// ============================================

void UISystem::RenderPanel(AestheticLayer& layer, int x, int y, int w, int h,
                           const std::string& title, bool focused) {
    // Panel background
    layer.RectFill(x, y, w, h, COLOR_PANEL_BG);
    
    // Border (2-pixel for retro feel)
    int borderColor = focused ? COLOR_TITLE_BG : COLOR_PANEL_BORDER;
    layer.Rect(x, y, w, h, borderColor);
    layer.Rect(x + 1, y + 1, w - 2, h - 2, borderColor);
    
    // Title bar
    if (!title.empty()) {
        int titleBarHeight = 8;
        layer.RectFill(x + 2, y + 2, w - 4, titleBarHeight, COLOR_TITLE_BG);
        
        // Title text (centered)
        int textWidth = GetTextWidth(title);
        int textX = x + (w - textWidth) / 2;
        int textY = y + 4;
        layer.Print(title, textX, textY, COLOR_TITLE_TEXT);
    }
}

void UISystem::RenderBox(AestheticLayer& layer, int x, int y, int w, int h, int color) {
    layer.RectFill(x, y, w, h, color);
    layer.Rect(x, y, w, h, COLOR_LIGHT_GRAY);
}

void UISystem::RenderSeparator(AestheticLayer& layer, int x, int y, int w, int color) {
    layer.Line(x, y, x + w, y, color);
}

// ============================================
// TEXT RENDERING
// ============================================

int UISystem::RenderText(AestheticLayer& layer, const std::string& text,
                         int x, int y, int maxWidth, int color) {
    std::vector<std::string> lines = WrapText(text, maxWidth);
    
    int lineHeight = GetCharHeight() + 2;  // 6px char + 2px spacing
    int currentY = y;
    
    for (const auto& line : lines) {
        layer.Print(line, x, currentY, color);
        currentY += lineHeight;
    }
    
    return lines.size();
}

void UISystem::RenderLine(AestheticLayer& layer, const std::string& text,
                          int x, int y, int color) {
    layer.Print(text, x, y, color);
}

void UISystem::RenderCenteredText(AestheticLayer& layer, const std::string& text,
                                  int x, int y, int w, int h, int color) {
    int textWidth = GetTextWidth(text);
    int textHeight = GetCharHeight();
    
    int textX = x + (w - textWidth) / 2;
    int textY = y + (h - textHeight) / 2;
    
    layer.Print(text, textX, textY, color);
}

// ============================================
// BUTTON RENDERING
// ============================================

bool UISystem::RenderButton(AestheticLayer& layer, const std::string& text,
                            int x, int y, int w, int h, bool pressed) {
    // Button background
    int bgColor = pressed ? COLOR_BUTTON_PRESSED : COLOR_BUTTON;
    layer.RectFill(x, y, w, h, bgColor);
    
    // Border
    layer.Rect(x, y, w, h, COLOR_LIGHT_GRAY);
    
    // Text (centered)
    RenderCenteredText(layer, text, x, y, w, h, COLOR_WHITE);
    
    // TODO: Return actual hover state when we integrate mouse
    return false;
}

// ============================================
// INPUT UTILITIES
// ============================================

bool UISystem::IsMouseInRect(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

int UISystem::GetTextWidth(const std::string& text) const {
    return text.length() * GetCharWidth();
}

// ============================================
// PRIVATE HELPERS
// ============================================

std::vector<std::string> UISystem::WrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    
    if (text.empty()) {
        return lines;
    }
    
    // Split by newlines first
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        // If line fits, add it directly
        if (GetTextWidth(line) <= maxWidth) {
            lines.push_back(line);
            continue;
        }
        
        // Otherwise, wrap by words
        std::istringstream wordStream(line);
        std::string word;
        std::string currentLine;
        
        while (wordStream >> word) {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            
            if (GetTextWidth(testLine) <= maxWidth) {
                currentLine = testLine;
            } else {
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                }
                currentLine = word;
            }
        }
        
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
    }
    
    return lines;
}
