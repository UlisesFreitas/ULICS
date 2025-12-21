#include "core/Bootstrap.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <shlobj.h>  // For SHGetFolderPath (Windows)

namespace Bootstrap {

std::string GetBootPath() {
    // Get AppData/Roaming path (Windows)
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        std::string appData(path);
        return appData + "\\com.ulics.dev\\ULICS\\.ulics_boot";
    }
    
    // Fallback to local directory if SHGetFolderPath fails
    return "./.ulics_boot";
}

void EnsureBootFiles() {
    std::string bootPath = GetBootPath();
    
    // Create boot directory if it doesn't exist
    try {
        std::filesystem::create_directories(bootPath);
        std::cout << "[Bootstrap] Boot directory: " << bootPath << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Bootstrap] ERROR: Failed to create boot directory: " << e.what() << std::endl;
        return;
    }
   
    // === main.lua ===
    std::string mainLuaPath = bootPath + "/main.lua";
    if (!std::filesystem::exists(mainLuaPath)) {
        std::cout << "[Bootstrap] Creating default main.lua..." << std::endl;
        std::ofstream mainLua(mainLuaPath);
        if (mainLua.is_open()) {
            // Simple boot script
            mainLua << "-- ULICS Boot Script\n";
            mainLua << "-- This is the default boot cartridge\n\n";
            mainLua << "function _init()\n";
            mainLua << "    print(\"ULICS System Ready\", 10, 10, 7)\n";
            mainLua << "end\n\n";
            mainLua << "function _update()\n";
            mainLua << "end\n\n";
            mainLua << "function _draw()\n";
            mainLua << "    cls(1)\n";
            mainLua << "    print(\"ULICS FANTASY CONSOLE\", 60, 100, 7)\n";
            mainLua << "    print(\"Press F1 for Code Editor\", 50, 120, 6)\n";
            mainLua << "end\n";
            mainLua.close();
            std::cout << "[Bootstrap] Created: " << mainLuaPath << std::endl;
        } else {
            std::cerr << "[Bootstrap] ERROR: Could not create main.lua" << std::endl;
        }
    }
    
    // === config.json ===
    std::string configPath = bootPath + "/config.json";
    if (!std::filesystem::exists(configPath)) {
        std::cout << "[Bootstrap] Creating default config.json..." << std::endl;
        std::ofstream config(configPath);
        if (config.is_open()) {
            config << "{\n";
            config << "  \"name\": \"ULICS Boot\",\n";
            config << "  \"author\": \"System\",\n";
            config << "  \"version\": \"1.0.0\",\n";
            config << "  \"description\": \"Default boot configuration\"\n";
            config << "}\n";
            config.close();
            std::cout << "[Bootstrap] Created: " << configPath << std::endl;
        } else {
            std::cerr << "[Bootstrap] ERROR: Could not create config.json" << std::endl;
        }
    }
}

void Initialize() {
    std::cout << "[Bootstrap] Initializing boot system..." << std::endl;
    EnsureBootFiles();
    std::cout << "[Bootstrap] Boot system ready" << std::endl;
}

} // namespace Bootstrap
