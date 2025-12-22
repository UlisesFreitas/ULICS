#include "core/Settings.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <cstdlib>

// Simple JSON-like parsing (manual, no external lib)

Settings::Settings() {
    ResetToDefaults();
}

Settings::~Settings() {
}

void Settings::ResetToDefaults() {
    volume = 70;
    fullscreen = false;
    debugMode = false;
    vsync = true;
}

void Settings::SetVolume(int vol) {
    if (vol < 0) vol = 0;
    if (vol > 100) vol = 100;
    volume = vol;
}

std::string Settings::GetSettingsPath() const {
    // Get AppData/Roaming path
    const char* appdata = std::getenv("APPDATA");
    if (!appdata) {
        std::cerr << "[Settings] APPDATA not found, using current directory" << std::endl;
        return "./settings.json";
    }
    
    std::filesystem::path settingsDir = std::filesystem::path(appdata) / "ULICS";
    
    // Create directory if it doesn't exist
    try {
        if (!std::filesystem::exists(settingsDir)) {
            std::filesystem::create_directories(settingsDir);
            std::cout << "[Settings] Created directory: " << settingsDir << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Settings] Failed to create directory: " << e.what() << std::endl;
        return "./settings.json";
    }
    
    return (settingsDir / "settings.json").string();
}

bool Settings::Load() {
    std::string path = GetSettingsPath();
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cout << "[Settings] No settings file found, using defaults" << std::endl;
        return false;
    }
    
    std::cout << "[Settings] Loading from: " << path << std::endl;
    
    // Simple manual JSON parsing (no external lib)
    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        
        // Skip comments and braces
        if (line[0] == '{' || line[0] == '}' || line[0] == '/' || line.empty()) {
            continue;
        }
        
        // Parse "key": value format
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Remove quotes and whitespace
        auto removeQuotes = [](std::string& s) {
            s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
            s.erase(std::remove(s.begin(), s.end(), ','), s.end());
            s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
            s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
            s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
            s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        };
        
        removeQuotes(key);
        removeQuotes(value);
        
        // Parse values
        if (key == "volume") {
            volume = std::stoi(value);
        }
        else if (key == "fullscreen") {
            fullscreen = (value == "true");
        }
        else if (key == "debugMode") {
            debugMode = (value == "true");
        }
        else if (key == "vsync") {
            vsync = (value == "true");
        }
    }
    
    file.close();
    
    std::cout << "[Settings] Loaded successfully" << std::endl;
    std::cout << "  Volume: " << volume << std::endl;
    std::cout << "  Fullscreen: " << (fullscreen ? "ON" : "OFF") << std::endl;
    std::cout << "  Debug Mode: " << (debugMode ? "ON" : "OFF") << std::endl;
    std::cout << "  VSync: " << (vsync ? "ON" : "OFF") << std::endl;
    
    return true;
}

bool Settings::Save() {
    std::string path = GetSettingsPath();
    std::ofstream file(path);
    
    if (!file.is_open()) {
        std::cerr << "[Settings] Failed to open file for writing: " << path << std::endl;
        return false;
    }
    
    std::cout << "[Settings] Saving to: " << path << std::endl;
    
    // Write JSON manually
    file << "{\n";
    file << "  \"volume\": " << volume << ",\n";
    file << "  \"fullscreen\": " << (fullscreen ? "true" : "false") << ",\n";
    file << "  \"debugMode\": " << (debugMode ? "true" : "false") << ",\n";
    file << "  \"vsync\": " << (vsync ? "true" : "false") << "\n";
    file << "}\n";
    
    file.close();
    
    std::cout << "[Settings] Saved successfully" << std::endl;
    return true;
}
