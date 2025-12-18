// IMPORTANT: Define NOMINMAX before windows.h
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "utils/FileDialog.h"
#include <windows.h>

namespace FileDialog {

std::string OpenFile(const char* filter, const char* title) {
    char filename[MAX_PATH] = {0};
    
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    
    return "";  // User cancelled
}

std::string SaveFile(const std::string& defaultName, const char* filter, 
                    const char* title, const char* defaultExt) {
    char filename[MAX_PATH] = {0};
    if (!defaultName.empty()) {
        strcpy_s(filename, MAX_PATH, defaultName.c_str());
    }
    
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title;
    ofn.lpstrDefExt = defaultExt;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    if (GetSaveFileNameA(&ofn)) {
        return std::string(filename);
    }
    
    return "";  // User cancelled
}

} // namespace FileDialog
