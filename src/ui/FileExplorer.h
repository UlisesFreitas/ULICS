#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <string>
#include <vector>
#include <filesystem>

// Forward declarations
class AestheticLayer;
class InputManager;
class UISystem;

/**
 * @brief File entry in the explorer tree
 */
struct FileEntry {
    std::string name;           // Display name (e.g., "main.lua")
    std::string fullPath;       // Absolute path
    bool isDirectory;           // Is this a directory?
    int depth;                  // Indentation level (0 = root)
    std::string icon;           // Icon character ("L", "J", etc.)
};

/**
 * @brief File Explorer Sidebar for Code Editor
 * 
 * Displays a tree view of .lua and .json files in the current cartridge directory.
 * Toggle with Ctrl+L, navigate with arrows, select with Enter.
 * 
 * Phase 2.0.5.2
 */
class FileExplorer {
public:
    FileExplorer();
    ~FileExplorer();

    // ============================================
    // CORE FUNCTIONS
    // ============================================
    
    /**
     * @brief Scan a directory for .lua and .json files
     * @param cartridgePath Path to cartridge directory (e.g., "cartridges/mygame")
     */
    void ScanDirectory(const std::string& cartridgePath);
    
    /**
     * @brief Render the sidebar
     * @param layer Rendering layer
     * @param x X position (typically 0)
     * @param y Y position (typically EDITOR_TOP)
     * @param width Sidebar width (typically 60px)
     * @param height Sidebar height (typically EDITOR_HEIGHT)
     */
    void Render(AestheticLayer& layer, UISystem& ui, int x, int y, int width, int height);
    
    /**
     * @brief Handle input (arrow keys, Enter)
     * @param input Input manager
     */
    void HandleInput(InputManager& input);
    
    // ============================================
    // STATE MANAGEMENT
    // ============================================
    
    /**
     * @brief Get the selected file path (empty if nothing selected this frame)
     */
    std::string GetSelectedFile() const;
    
    /**
     * @brief Set the current file (highlights it in the list)
     */
    void SetCurrentFile(const std::string& filepath);
    
    /**
     * @brief Check if sidebar is visible
     */
    bool IsVisible() const { return visible; }
    
    /**
     * @brief Set sidebar visibility
     */
    void SetVisible(bool vis) { visible = vis; }
    
    /**
     * @brief Toggle sidebar visibility
     */
    void Toggle() { visible = !visible; }
    
    /**
     * @brief Clear selected file (reset after file loaded)
     */
    void ClearSelection() { fileJustSelected = false; }

private:
    // File list
    std::vector<FileEntry> files;
    
    // Selection state
    int selectedIndex;          // Currently highlighted file
    int scrollOffset;           // Scroll position (for many files)
    bool visible;               // Is sidebar visible?
    std::string currentFile;    // Currently open file (highlighted differently)
    bool fileJustSelected;      // Was file selected this frame?
    std::string pendingFile;    // File to open
    
    // Input state (for key repeat)
    int upKeyHoldFrames;
    int downKeyHoldFrames;
    int keyRepeatDelay;         // Frames before repeat
    int keyRepeatInterval;      // Frames between repeats
    
    // ============================================
    // HELPER FUNCTIONS
    // ============================================
    
    /**
     * @brief Recursively scan directory
     */
    void ScanRecursive(const std::filesystem::path& path, int depth);
    
    /**
     * @brief Check if file is .lua or .json
     */
    bool IsLuaOrJson(const std::string& filename) const;
    
    /**
     * @brief Get file icon based on extension
     */
    std::string GetFileIcon(const std::string& extension) const;
    
    /**
     * @brief Ensure selected index is visible (scroll if needed)
     */
    void EnsureSelectionVisible(int visibleLines);
    
    /**
     * @brief Clamp selection to valid range
     */
    void ClampSelection();
};

#endif // FILEEXPLORER_H
