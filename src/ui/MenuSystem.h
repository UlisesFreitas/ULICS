#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string>
#include <vector>
#include <functional>

// Forward declarations
class AestheticLayer;
class InputManager;

/**
 * @brief MenuSystem - Generic menu system for ULICS
 * 
 * Provides a flexible menu system with:
 * - Text items with callbacks
 * - Sliders for settings
 * - Keyboard navigation (UP/DOWN/ENTER/ESC)
 * - RGB rendering with SystemColors
 * 
 * Used for: Main Menu, Pause Menu, Settings Menu
 */
class MenuSystem {
public:
    /**
     * @brief Menu item types
     */
    enum class ItemType {
        TEXT,       // Simple text with callback
        SLIDER,     // Integer slider (for settings)
        TOGGLE,     // Boolean toggle
        SEPARATOR   // Visual separator
    };
    
    /**
     * @brief Menu item structure
     */
    struct MenuItem {
        ItemType type;
        std::string text;
        bool enabled;
        
        // For TEXT items
        std::function<void()> action;
        
        // For SLIDER items
        int* valuePtr;
        int minValue;
        int maxValue;
        std::string unit;  // e.g., "%" for volume
        
        // For TOGGLE items
        bool* togglePtr;
        
        MenuItem() 
            : type(ItemType::TEXT), enabled(true), 
              action(nullptr), valuePtr(nullptr), 
              minValue(0), maxValue(100), togglePtr(nullptr) {}
    };
    
    MenuSystem();
    ~MenuSystem();
    
    // === Menu Construction ===
    
    /**
     * @brief Set menu title
     */
    void SetTitle(const std::string& title);
    
    /**
     * @brief Add a text item with callback
     */
    void AddItem(const std::string& text, std::function<void()> action, bool enabled = true);
    
    /**
     * @brief Add a slider item
     */
    void AddSlider(const std::string& text, int* valuePtr, int min, int max, 
                   const std::string& unit = "", bool enabled = true);
    
    /**
     * @brief Add a toggle item
     */
    void AddToggle(const std::string& text, bool* togglePtr, bool enabled = true);
    
    /**
     * @brief Add a visual separator
     */
    void AddSeparator();
    
    /**
     * @brief Clear all items
     */
    void Clear();
    
    // === Input Handling ===
    
    /**
     * @brief Update menu state with input
     * @return true if menu is still active, false if ESC was pressed
     */
    bool Update(InputManager& input);
    
    // === Rendering ===
    
    /**
     * @brief Render the menu
     */
    void Render(AestheticLayer& layer);
    
    // === State ===
    
    /**
     * @brief Get currently selected index
     */
    int GetSelectedIndex() const { return selectedIndex; }
    
    /**
     * @brief Set selected index
     */
    void SetSelectedIndex(int index);
    
    /**
     * @brief Check if menu is visible
     */
    bool IsVisible() const { return visible; }
    
    /**
     * @brief Set menu visibility
     */
    void SetVisible(bool vis) { visible = vis; }
    
    /**
     * @brief Get number of items
     */
    int GetItemCount() const { return static_cast<int>(items.size()); }
    
private:
    std::string title;
    std::vector<MenuItem> items;
    int selectedIndex;
    bool visible;
    
    // Input state (for key repeat)
    int upHoldFrames;
    int downHoldFrames;
    int leftHoldFrames;
    int rightHoldFrames;
    
    static constexpr int KEY_REPEAT_DELAY = 15;     // Frames to wait before repeat
    static constexpr int KEY_REPEAT_INTERVAL = 3;   // Frames between repeats
    
    // Helper functions
    void MoveUp();
    void MoveDown();
    void ActivateSelected();
    void AdjustSlider(int delta);
    void ToggleSelected();
    
    void RenderMenuItem(AestheticLayer& layer, const MenuItem& item, 
                       int x, int y, bool selected);
};

#endif // MENUSYSTEM_H
