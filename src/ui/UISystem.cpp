#include "ui/UISystem.h"
#include "ui/SystemColors.h"  // Fixed UI colors
#include "rendering/AestheticLayer.h"
#include <sstream>
#include <algorithm>

UISystem::UISystem() {
}

UISystem::~UISystem() {
}

// ============================================
// NOTE: UISystem currently disabled - needs refactor to SystemColors RGB
// These functions use indexed colors which change with palette imports
// TODO: Refactor to use PrintRGB, RectRGB, etc. with SystemColors
// ============================================

// Placeholder implementations to allow compilation
void UISystem::RenderPanel(AestheticLayer& layer, int x, int y, int w, int h,
                           const std::string& title, bool focused) {
    // TODO: Implement with RGB
}

void UISystem::RenderBox(AestheticLayer& layer, int x, int y, int w, int h, int color) {
    // TODO: Implement with RGB
}

void UISystem::RenderSeparator(AestheticLayer& layer, int x, int y, int w, int color) {
    // TODO: Implement with RGB
}

int UISystem::RenderText(AestheticLayer& layer, const std::string& text,
                         int x, int y, int maxWidth, int color) {
    // TODO: Implement with RGB
    return 0;
}

void UISystem::RenderLine(AestheticLayer& layer, const std::string& text,
                          int x, int y, int color) {
    // TODO: Implement with RGB
}

void UISystem::RenderCenteredText(AestheticLayer& layer, const std::string& text,
                                  int x, int y, int w, int h, int color) {
    //TODO: Implement with RGB
}

bool UISystem::RenderButton(AestheticLayer& layer, const std::string& text,
                            int x, int y, int w, int h, bool pressed) {
    // TODO: Implement with RGB
    return false;
}

bool UISystem::IsMouseInRect(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

int UISystem::GetTextWidth(const std::string& text) const {
    return text.length() * GetCharWidth();
}

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
