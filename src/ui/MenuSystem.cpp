#include "ui/MenuSystem.h"
#include "ui/SystemColors.h"
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "input/InputConstants.h"
#include <algorithm>

MenuSystem::MenuSystem()
    : selectedIndex(0), visible(true),
      upHoldFrames(0), downHoldFrames(0), 
      leftHoldFrames(0), rightHoldFrames(0) {
}

MenuSystem::~MenuSystem() {
}

// === Menu Construction ===

void MenuSystem::SetTitle(const std::string& newTitle) {
    title = newTitle;
}

void MenuSystem::AddItem(const std::string& text, std::function<void()> action, bool enabled) {
    MenuItem item;
    item.type = ItemType::TEXT;
    item.text = text;
    item.action = action;
    item.enabled = enabled;
    items.push_back(item);
}

void MenuSystem::AddSlider(const std::string& text, int* valuePtr, int min, int max, 
                          const std::string& unit, bool enabled) {
    MenuItem item;
    item.type = ItemType::SLIDER;
    item.text = text;
    item.valuePtr = valuePtr;
    item.minValue = min;
    item.maxValue = max;
    item.unit = unit;
    item.enabled = enabled;
    items.push_back(item);
}

void MenuSystem::AddToggle(const std::string& text, bool* togglePtr, bool enabled) {
    MenuItem item;
    item.type = ItemType::TOGGLE;
    item.text = text;
    item.togglePtr = togglePtr;
    item.enabled = enabled;
    items.push_back(item);
}

void MenuSystem::AddSeparator() {
    MenuItem item;
    item.type = ItemType::SEPARATOR;
    item.text = "";
    item.enabled = false;
    items.push_back(item);
}

void MenuSystem::Clear() {
    items.clear();
    selectedIndex = 0;
}

void MenuSystem::SetSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(items.size())) {
        selectedIndex = index;
    }
}

// === Input Handling ===

bool MenuSystem::Update(InputManager& input) {
    if (!visible || items.empty()) {
        return true;
    }
    
    // UP - Navigate up
    if (input.isKeyDown(SDL_SCANCODE_UP) || input.isKeyDown(SDL_SCANCODE_W)) {
        upHoldFrames++;
        if (upHoldFrames == 1 || 
            (upHoldFrames > KEY_REPEAT_DELAY && 
             (upHoldFrames - KEY_REPEAT_DELAY) % KEY_REPEAT_INTERVAL == 0)) {
            MoveUp();
        }
    } else {
        upHoldFrames = 0;
    }
    
    // DOWN - Navigate down
    if (input.isKeyDown(SDL_SCANCODE_DOWN) || input.isKeyDown(SDL_SCANCODE_S)) {
        downHoldFrames++;
        if (downHoldFrames == 1 || 
            (downHoldFrames > KEY_REPEAT_DELAY && 
             (downHoldFrames - KEY_REPEAT_DELAY) % KEY_REPEAT_INTERVAL == 0)) {
            MoveDown();
        }
    } else {
        downHoldFrames = 0;
    }
    
    // LEFT - Decrease slider value
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) {
        const auto& item = items[selectedIndex];
        if (item.type == ItemType::SLIDER && item.enabled) {
            if (input.isKeyDown(SDL_SCANCODE_LEFT) || input.isKeyDown(SDL_SCANCODE_A)) {
                leftHoldFrames++;
                if (leftHoldFrames == 1 || 
                    (leftHoldFrames > KEY_REPEAT_DELAY && 
                     (leftHoldFrames - KEY_REPEAT_DELAY) % KEY_REPEAT_INTERVAL == 0)) {
                    AdjustSlider(-1);
                }
            } else {
                leftHoldFrames = 0;
            }
        }
    }
    
    // RIGHT - Increase slider value
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) {
        const auto& item = items[selectedIndex];
        if (item.type == ItemType::SLIDER && item.enabled) {
            if (input.isKeyDown(SDL_SCANCODE_RIGHT) || input.isKeyDown(SDL_SCANCODE_D)) {
                rightHoldFrames++;
                if (rightHoldFrames == 1 || 
                    (rightHoldFrames > KEY_REPEAT_DELAY && 
                     (rightHoldFrames - KEY_REPEAT_DELAY) % KEY_REPEAT_INTERVAL == 0)) {
                    AdjustSlider(1);
                }
            } else {
                rightHoldFrames = 0;
            }
        }
    }
    
    // ENTER / SPACE - Activate selected item
    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || 
        input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        ActivateSelected();
    }
    
    // ESC - Close menu
    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        return false;  // Menu wants to close
    }
    
    return true;  // Menu still active
}

// === Helper Functions ===

void MenuSystem::MoveUp() {
    if (items.empty()) return;
    
    int originalIndex = selectedIndex;
    do {
        selectedIndex--;
        if (selectedIndex < 0) {
            selectedIndex = static_cast<int>(items.size()) - 1;
        }
        
        // Skip separators and disabled items
        if (items[selectedIndex].type != ItemType::SEPARATOR && 
            items[selectedIndex].enabled) {
            break;
        }
        
        // Prevent infinite loop
        if (selectedIndex == originalIndex) break;
        
    } while (true);
}

void MenuSystem::MoveDown() {
    if (items.empty()) return;
    
    int originalIndex = selectedIndex;
    do {
        selectedIndex++;
        if (selectedIndex >= static_cast<int>(items.size())) {
            selectedIndex = 0;
        }
        
        // Skip separators and disabled items
        if (items[selectedIndex].type != ItemType::SEPARATOR && 
            items[selectedIndex].enabled) {
            break;
        }
        
        // Prevent infinite loop
        if (selectedIndex == originalIndex) break;
        
    } while (true);
}

void MenuSystem::ActivateSelected() {
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size())) {
        return;
    }
    
    MenuItem& item = items[selectedIndex];
    
    if (!item.enabled) return;
    
    switch (item.type) {
        case ItemType::TEXT:
            if (item.action) {
                item.action();
            }
            break;
            
        case ItemType::TOGGLE:
            ToggleSelected();
            break;
            
        case ItemType::SLIDER:
            // ENTER on slider doesn't do anything
            break;
            
        case ItemType::SEPARATOR:
            // Can't activate separator
            break;
    }
}

void MenuSystem::AdjustSlider(int delta) {
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size())) {
        return;
    }
    
    MenuItem& item = items[selectedIndex];
    
    if (item.type != ItemType::SLIDER || !item.enabled || !item.valuePtr) {
        return;
    }
    
    *item.valuePtr += delta;
    
    // Clamp to range
    if (*item.valuePtr < item.minValue) {
        *item.valuePtr = item.minValue;
    }
    if (*item.valuePtr > item.maxValue) {
        *item.valuePtr = item.maxValue;
    }
}

void MenuSystem::ToggleSelected() {
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size())) {
        return;
    }
    
    MenuItem& item = items[selectedIndex];
    
    if (item.type != ItemType::TOGGLE || !item.enabled || !item.togglePtr) {
        return;
    }
    
    *item.togglePtr = !(*item.togglePtr);
}

// === Rendering ===

void MenuSystem::Render(AestheticLayer& layer) {
    if (!visible) return;
    
    const int SCREEN_W = 256;
    const int SCREEN_H = 256;
    
    // Menu dimensions
    const int MENU_W = 180;
    const int MENU_H = 140;
    const int MENU_X = (SCREEN_W - MENU_W) / 2;
    const int MENU_Y = (SCREEN_H - MENU_H) / 2;
    
    // Draw menu background
    layer.RectFillRGB(MENU_X, MENU_Y, MENU_W, MENU_H,
                     SystemColors::BLACK.r, 
                     SystemColors::BLACK.g, 
                     SystemColors::BLACK.b);
    
    // Draw border
    layer.RectRGB(MENU_X, MENU_Y, MENU_W, MENU_H,
                 SystemColors::LIGHT_GRAY.r,
                 SystemColors::LIGHT_GRAY.g,
                 SystemColors::LIGHT_GRAY.b);
    
    // Draw title
    if (!title.empty()) {
        int titleX = MENU_X + (MENU_W - static_cast<int>(title.length()) * 8) / 2;
        int titleY = MENU_Y + 8;
        
        layer.PrintRGB(title.c_str(), titleX, titleY,
                      SystemColors::YELLOW.r,
                      SystemColors::YELLOW.g,
                      SystemColors::YELLOW.b);
        
        // Title separator
        layer.LineRGB(MENU_X + 4, MENU_Y + 20, MENU_X + MENU_W - 4, MENU_Y + 20,
                     SystemColors::LIGHT_GRAY.r,
                     SystemColors::LIGHT_GRAY.g,
                     SystemColors::LIGHT_GRAY.b);
    }
    
    // Draw items
    int itemY = MENU_Y + 30;
    for (size_t i = 0; i < items.size(); i++) {
        const auto& item = items[i];
        bool selected = (static_cast<int>(i) == selectedIndex);
        
        RenderMenuItem(layer, item, MENU_X + 10, itemY, selected);
        
        itemY += (item.type == ItemType::SEPARATOR) ? 5 : 12;
    }
    
    // Draw help text at bottom
    const char* helpText = "[↑↓] Navigate  [ENTER] Select  [ESC] Back";
    int helpX = MENU_X + (MENU_W - static_cast<int>(strlen(helpText)) * 4) / 2;
    int helpY = MENU_Y + MENU_H - 10;
    
    layer.PrintRGB(helpText, helpX, helpY,
                  SystemColors::LIGHT_GRAY.r,
                  SystemColors::LIGHT_GRAY.g,
                  SystemColors::LIGHT_GRAY.b);
}

void MenuSystem::RenderMenuItem(AestheticLayer& layer, const MenuItem& item, 
                               int x, int y, bool selected) {
    switch (item.type) {
        case ItemType::TEXT: {
            // Selection indicator
            if (selected) {
                layer.PrintRGB("►", x - 8, y,
                              SystemColors::YELLOW.r,
                              SystemColors::YELLOW.g,
                              SystemColors::YELLOW.b);
            }
            
            // Text
            SDL_Color textColor = item.enabled ? 
                (selected ? SystemColors::WHITE : SystemColors::LIGHT_GRAY) :
                SystemColors::DARK_GRAY;
                
            layer.PrintRGB(item.text.c_str(), x, y,
                          textColor.r, textColor.g, textColor.b);
            break;
        }
        
        case ItemType::SLIDER: {
            // Selection indicator
            if (selected) {
                layer.PrintRGB("►", x - 8, y,
                              SystemColors::YELLOW.r,
                              SystemColors::YELLOW.g,
                              SystemColors::YELLOW.b);
            }
            
            // Label
            SDL_Color textColor = item.enabled ? 
                (selected ? SystemColors::WHITE : SystemColors::LIGHT_GRAY) :
                SystemColors::DARK_GRAY;
                
            layer.PrintRGB(item.text.c_str(), x, y,
                          textColor.r, textColor.g, textColor.b);
            
            // Value and slider
            if (item.valuePtr) {
                char valueText[32];
                snprintf(valueText, sizeof(valueText), "%d%s", 
                        *item.valuePtr, item.unit.c_str());
                
                int valueX = x + 90;
                layer.PrintRGB(valueText, valueX, y,
                              SystemColors::CYAN.r,
                              SystemColors::CYAN.g,
                              SystemColors::CYAN.b);
                
                // Draw slider bar
                int barX = x + 120;
                int barY = y + 3;
                int barW = 40;
                int barH = 4;
                
                // Background
                layer.RectFillRGB(barX, barY, barW, barH,
                                 SystemColors::DARK_GRAY.r,
                                 SystemColors::DARK_GRAY.g,
                                 SystemColors::DARK_GRAY.b);
                
                // Fill
                int fillW = (barW * (*item.valuePtr - item.minValue)) / 
                           (item.maxValue - item.minValue);
                if (fillW > 0) {
                    layer.RectFillRGB(barX, barY, fillW, barH,
                                     SystemColors::GREEN.r,
                                     SystemColors::GREEN.g,
                                     SystemColors::GREEN.b);
                }
                
                // Border
                layer.RectRGB(barX, barY, barW, barH,
                             SystemColors::LIGHT_GRAY.r,
                             SystemColors::LIGHT_GRAY.g,
                             SystemColors::LIGHT_GRAY.b);
            }
            break;
        }
        
        case ItemType::TOGGLE: {
            // Selection indicator
            if (selected) {
                layer.PrintRGB("►", x - 8, y,
                              SystemColors::YELLOW.r,
                              SystemColors::YELLOW.g,
                              SystemColors::YELLOW.b);
            }
            
            // Label
            SDL_Color textColor = item.enabled ? 
                (selected ? SystemColors::WHITE : SystemColors::LIGHT_GRAY) :
                SystemColors::DARK_GRAY;
                
            layer.PrintRGB(item.text.c_str(), x, y,
                          textColor.r, textColor.g, textColor.b);
            
            // Toggle state
            if (item.togglePtr) {
                const char* stateText = *item.togglePtr ? "[ ON  ]" : "[ OFF ]";
                SDL_Color stateColor = *item.togglePtr ? 
                    SystemColors::GREEN : SystemColors::RED;
                
                int stateX = x + 100;
                layer.PrintRGB(stateText, stateX, y,
                              stateColor.r, stateColor.g, stateColor.b);
            }
            break;
        }
        
        case ItemType::SEPARATOR: {
            // Draw horizontal line
            layer.LineRGB(x - 5, y + 2, x + 150, y + 2,
                         SystemColors::DARK_GRAY.r,
                         SystemColors::DARK_GRAY.g,
                         SystemColors::DARK_GRAY.b);
            break;
        }
    }
}
