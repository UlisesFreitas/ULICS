#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <string>

// Cross-platform (well, Windows for now) file dialog utilities
namespace FileDialog {
    // Open file dialog - returns selected file path or empty string if cancelled
    std::string OpenFile(const char* filter = "All Files\0*.*\0", 
                        const char* title = "Open File");
    
    // Save file dialog - returns selected file path or empty string if cancelled
    std::string SaveFile(const std::string& defaultName,
                        const char* filter = "All Files\0*.*\0",
                        const char* title = "Save File",
                        const char* defaultExt = nullptr);
}

#endif // FILE_DIALOG_H
