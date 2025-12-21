#include "ui/FileExplorer.h"
#include "ui/SystemColors.h"  // Fixed UI colors
#include "rendering/AestheticLayer.h"
#include "input/InputManager.h"
#include "input/InputConstants.h"
#include <algorithm>

FileExplorer::FileExplorer()
    : selectedIndex(0), scrollOffset(0), visible(false), fileJustSelected(false),
      upKeyHoldFrames(0), downKeyHoldFrames(0), 
      keyRepeatDelay(20), keyRepeatInterval(3) {
}

FileExplorer::~FileExplorer() {
}

// ============================================
// CORE FUNCTIONS
// ============================================

void FileExplorer::ScanDirectory(const std::string& cartridgePath) {
    files.clear();
    selectedIndex = 0;
    scrollOffset = 0;
    
    if (cartridgePath.empty()) {
        return;
    }
    
    try {
        std::filesystem::path path(cartridgePath);
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            ScanRecursive(path, 0);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Ignore errors - just show empty list
    }
    
    // Sort files: directories first, then alphabetically
    std::sort(files.begin(), files.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory > b.isDirectory;
        }
        return a.name < b.name;
    });
}

void FileExplorer::Render(AestheticLayer& layer, UISystem& ui, int x, int y, int width, int height) {
    if (!visible) {
        return;
    }
    
    // Sidebar background (dark gray, RGB fixed)
    layer.RectFillRGB(x, y, width, height,
                     SystemColors::DARK_GRAY.r, SystemColors::DARK_GRAY.g, SystemColors::DARK_GRAY.b);
    
    // Border (right edge, light gray, RGB fixed)
    layer.LineRGB(x + width - 1, y, x + width - 1, y + height - 1,
                 SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
    
    const int CHAR_H = 8;
    const int LINE_H = 10;  // 8px font + 2px spacing
    const int VISIBLE_LINES = height / LINE_H;
    
    // If no files, show message
    if (files.empty()) {
        layer.PrintRGB("No files", x + 4, y + 12,
                      SystemColors::LIGHT_GRAY.r, SystemColors::LIGHT_GRAY.g, SystemColors::LIGHT_GRAY.b);
        return;
    }
    
    // Render files
    int renderY = y + 2;
    int itemIndex = 0;
    
    for (size_t i = scrollOffset; i < files.size(); i++) {
        if (itemIndex >= VISIBLE_LINES) {
            break;
        }
        
        const FileEntry& entry = files[i];
        
        // Selection highlight
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        bool isCurrent = (entry.fullPath == currentFile);
        
        SDL_Color bgColor = SystemColors::DARK_GRAY;
        SDL_Color textColor = SystemColors::WHITE;
        SDL_Color iconColor = SystemColors::YELLOW;
        
        if (isSelected) {
            bgColor = SystemColors::LAVENDER;
            textColor = SystemColors::WHITE;
        } else if (isCurrent) {
            bgColor = SystemColors::DARK_PURPLE;
            textColor = SystemColors::WHITE;
        }
        
        // Draw background for this item
        if (isSelected || isCurrent) {
            layer.RectFillRGB(x, renderY - 1, width - 1, LINE_H,
                             bgColor.r, bgColor.g, bgColor.b);
        }
        
        // Indent for depth
        int indentX = x + 2 + (entry.depth * 6);
        
        // Icon
        if (!entry.icon.empty()) {
            layer.PrintRGB(entry.icon.c_str(), indentX, renderY,
                          iconColor.r, iconColor.g, iconColor.b);
            indentX += 8;  // Move past icon
        }
        
        // File name (truncate if too long)
        std::string displayName = entry.name;
        int availableWidth = width - (indentX - x) - 4;
        int maxChars = availableWidth / 4;  // 4px per char (CHAR_W from EmbeddedFont)
        if (maxChars < 1) maxChars = 1;
        
        if (displayName.length() > static_cast<size_t>(maxChars)) {
            displayName = displayName.substr(0, maxChars - 1) + "~";
        }
        
        layer.PrintRGB(displayName.c_str(), indentX, renderY,
                      textColor.r, textColor.g, textColor.b);
        
        renderY += LINE_H;
        itemIndex++;
    }
    
    // Scroll indicators
    if (scrollOffset > 0) {
        // Up arrow (yellow, RGB fixed)
        layer.PrintRGB("^", x + width - 8, y + 2,
                      SystemColors::YELLOW.r, SystemColors::YELLOW.g, SystemColors::YELLOW.b);
    }
    if (scrollOffset + VISIBLE_LINES < static_cast<int>(files.size())) {
        // Down arrow (yellow, RGB fixed)
        layer.PrintRGB("v", x + width - 8, y + height - 10,
                      SystemColors::YELLOW.r, SystemColors::YELLOW.g, SystemColors::YELLOW.b);
    }
}

void FileExplorer::HandleInput(InputManager& input) {
    if (!visible || files.empty()) {
        return;
    }
    
    fileJustSelected = false;
    
    // Up arrow - with auto-repeat
    if (input.isKeyDown(SDL_SCANCODE_UP)) {
        upKeyHoldFrames++;
        if (upKeyHoldFrames == 1 || 
            (upKeyHoldFrames > keyRepeatDelay && (upKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            if (selectedIndex > 0) {
                selectedIndex--;
                EnsureSelectionVisible(20);  // Approximate visible lines
            }
        }
    } else {
        upKeyHoldFrames = 0;
    }
    
    // Down arrow - with auto-repeat
    if (input.isKeyDown(SDL_SCANCODE_DOWN)) {
        downKeyHoldFrames++;
        if (downKeyHoldFrames == 1 || 
            (downKeyHoldFrames > keyRepeatDelay && (downKeyHoldFrames - keyRepeatDelay) % keyRepeatInterval == 0)) {
            if (selectedIndex < static_cast<int>(files.size()) - 1) {
                selectedIndex++;
                EnsureSelectionVisible(20);
            }
        }
    } else {
        downKeyHoldFrames = 0;
    }
    
    // Enter - select file
    if (input.isKeyPressed(SDL_SCANCODE_RETURN)) {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(files.size())) {
            const FileEntry& entry = files[selectedIndex];
            if (!entry.isDirectory) {
                pendingFile = entry.fullPath;
                fileJustSelected = true;
            }
        }
    }
}

// ============================================
// STATE MANAGEMENT
// ============================================

std::string FileExplorer::GetSelectedFile() const {
    if (fileJustSelected) {
        return pendingFile;
    }
    return "";
}

void FileExplorer::SetCurrentFile(const std::string& filepath) {
    currentFile = filepath;
    
    // Try to select this file in the list
    for (size_t i = 0; i < files.size(); i++) {
        if (files[i].fullPath == filepath) {
            selectedIndex = static_cast<int>(i);
            EnsureSelectionVisible(20);
            break;
        }
    }
}

// ============================================
// HELPER FUNCTIONS
// ============================================

void FileExplorer::ScanRecursive(const std::filesystem::path& path, int depth) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                // Add directory
                FileEntry dirEntry;
                dirEntry.name = entry.path().filename().string();
                dirEntry.fullPath = entry.path().string();
                dirEntry.isDirectory = true;
                dirEntry.depth = depth;
                dirEntry.icon = "D";
                files.push_back(dirEntry);
                
                // Recurse into subdirectory
                ScanRecursive(entry.path(), depth + 1);
            } else if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (IsLuaOrJson(filename)) {
                    FileEntry fileEntry;
                    fileEntry.name = filename;
                    fileEntry.fullPath = entry.path().string();
                    fileEntry.isDirectory = false;
                    fileEntry.depth = depth;
                    fileEntry.icon = GetFileIcon(entry.path().extension().string());
                    files.push_back(fileEntry);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Skip directories we can't read
    }
}

bool FileExplorer::IsLuaOrJson(const std::string& filename) const {
    if (filename.length() < 4) {
        return false;
    }
    
    std::string lower = filename;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Check for .lua
    if (lower.length() >= 4 && lower.substr(lower.length() - 4) == ".lua") {
        return true;
    }
    
    // Check for .json
    if (lower.length() >= 5 && lower.substr(lower.length() - 5) == ".json") {
        return true;
    }
    
    return false;
}

std::string FileExplorer::GetFileIcon(const std::string& extension) const {
    std::string lower = extension;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == ".lua") {
        return "L";
    } else if (lower == ".json") {
        return "J";
    }
    return "?";
}

void FileExplorer::EnsureSelectionVisible(int visibleLines) {
    // Scroll up if selection is above visible area
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    }
    
    // Scroll down if selection is below visible area
    if (selectedIndex >= scrollOffset + visibleLines) {
        scrollOffset = selectedIndex - visibleLines + 1;
    }
    
    // Clamp scroll
    if (scrollOffset < 0) {
        scrollOffset = 0;
    }
}

void FileExplorer::ClampSelection() {
    if (files.empty()) {
        selectedIndex = 0;
        return;
    }
    
    if (selectedIndex < 0) {
        selectedIndex = 0;
    }
    if (selectedIndex >= static_cast<int>(files.size())) {
        selectedIndex = files.size() - 1;
    }
}
