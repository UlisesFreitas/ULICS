#include "cartridge/CartridgeLoader.h"
#include <json.hpp> // nlohmann/json
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::vector<CartridgeLoader::CartridgeInfo> CartridgeLoader::ListAvailableCartridges(const std::string& directory_path) {
    std::vector<CartridgeInfo> cartridges;
    last_error.clear();

    if (!DirectoryExists(directory_path)) {
        // v1.1.6: Better error message
        last_error = "Directory does not exist: " + directory_path;
        std::cerr << "\n=== ULICS Error ===" << std::endl;
        std::cerr << "Cartridge directory not found: " << directory_path << std::endl;
        std::cerr << "\nTo fix:" << std::endl;
        std::cerr << "  1. Create the 'cartridges' folder if it doesn't exist" << std::endl;
        std::cerr << "  2. Add at least one cartridge subfolder with main.lua" << std::endl;
        std::cerr << "\nExample structure:" << std::endl;
        std::cerr << "  cartridges/" << std::endl;
        std::cerr << "    my_game/" << std::endl;
        std::cerr << "      main.lua" << std::endl;
        std::cerr << "      config.json" << std::endl;
        std::cerr << "==================\n" << std::endl;
        return cartridges;
    }

    try {
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            CartridgeInfo info;
            
            if (entry.is_directory()) {
                // Check if directory contains main.lua
                std::string main_lua = entry.path().string() + "/main.lua";
                if (FileExists(main_lua)) {
                    info.path = entry.path().string();
                    info.main_lua_path = main_lua;
                    
                    // Check for config.json
                    std::string config_path = entry.path().string() + "/config.json";
                    info.has_config = FileExists(config_path);
                    if (info.has_config) {
                        info.config_path = config_path;
                        
                        // Try to get name from config
                        auto config = ParseConfig(config_path);
                        info.name = config.name;
                    } else {
                        // Use directory name
                        info.name = ExtractCartridgeName(info.path);
                    }
                    
                    cartridges.push_back(info);
                }
            } else if (entry.is_regular_file() && entry.path().extension() == ".lua") {
                // Single .lua file
                info.path = entry.path().string();
                info.main_lua_path = info.path;
                info.has_config = false;
                info.name = ExtractCartridgeName(info.path);
                
                cartridges.push_back(info);
            }
        }
    } catch (const fs::filesystem_error& e) {
        last_error = "Filesystem error: " + std::string(e.what());
        std::cerr << "CartridgeLoader: " << last_error << std::endl;
    }

    std::cout << "CartridgeLoader: Found " << cartridges.size() << " cartridge(s) in " << directory_path << std::endl;
    return cartridges;
}

CartridgeConfig CartridgeLoader::ParseConfig(const std::string& config_path) {
    CartridgeConfig config = CartridgeConfig::Default();
    last_error.clear();

    if (!FileExists(config_path)) {
        // Not an error - config.json is optional
        return config;
    }

    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            last_error = "Could not open config file: " + config_path;
            std::cerr << "CartridgeLoader: " << last_error << std::endl;
            return config;
        }

        json j;
        file >> j;

        // Parse metadata (optional fields)
        if (j.contains("name") && j["name"].is_string()) {
            config.name = j["name"].get<std::string>();
        }
        
        if (j.contains("author") && j["author"].is_string()) {
            config.author = j["author"].get<std::string>();
        }
        
        if (j.contains("version") && j["version"].is_string()) {
            config.version = j["version"].get<std::string>();
        }
        
        if (j.contains("description") && j["description"].is_string()) {
            config.description = j["description"].get<std::string>();
        }

        // Parse resource limits (CRITICAL)
        if (j.contains("memory_limit_mb") && j["memory_limit_mb"].is_number_integer()) {
            config.memory_limit_mb = j["memory_limit_mb"].get<int>();
        }
        
        if (j.contains("lua_code_limit_lines") && j["lua_code_limit_lines"].is_number_integer()) {
            config.lua_code_limit_lines = j["lua_code_limit_lines"].get<int>();
        }

        // Parse graphics configuration
        if (j.contains("palette_size") && j["palette_size"].is_number_integer()) {
            config.palette_size = j["palette_size"].get<int>();
        }
        
        if (j.contains("framebuffer_width") && j["framebuffer_width"].is_number_integer()) {
            config.framebuffer_width = j["framebuffer_width"].get<int>();
        }
        
        if (j.contains("framebuffer_height") && j["framebuffer_height"].is_number_integer()) {
            config.framebuffer_height = j["framebuffer_height"].is_number_integer();
        }

        // Parse performance
        if (j.contains("target_fps") && j["target_fps"].is_number_integer()) {
            config.target_fps = j["target_fps"].get<int>();
        }

        // Validate and clamp all values
        if (!config.Validate()) {
            std::cout << "CartridgeLoader: Some config values were out of range and have been clamped." << std::endl;
        }

        std::cout << "CartridgeLoader: Loaded config from " << config_path << std::endl;
        std::cout << "  - Name: " << config.name << std::endl;
        std::cout << "  - Memory Limit: " << config.memory_limit_mb << " MB" << std::endl;
        std::cout << "  - Code Limit: " << config.lua_code_limit_lines << " lines" << std::endl;
        std::cout << "  - Palette Size: " << config.palette_size << " colors" << std::endl;

    } catch (const json::parse_error& e) {
        last_error = "JSON parse error: " + std::string(e.what());
        std::cerr << "CartridgeLoader: " << last_error << std::endl;
        // Return default config on parse error
    } catch (const std::exception& e) {
        last_error = "Error reading config: " + std::string(e.what());
        std::cerr << "CartridgeLoader: " << last_error << std::endl;
    }

    return config;
}

CartridgeConfig CartridgeLoader::LoadCartridge(const std::string& cartridge_path) {
    last_error.clear();

    if (!IsValidCartridge(cartridge_path)) {
        last_error = "Not a valid cartridge: " + cartridge_path;
        std::cerr << "CartridgeLoader: " << last_error << std::endl;
        return CartridgeConfig::Default();
    }

    // Check if it's a .lua file or directory
    if (cartridge_path.find(".lua") != std::string::npos) {
        // Single .lua file - no config
        std::cout << "CartridgeLoader: Loading single .lua file: " << cartridge_path << std::endl;
        return CartridgeConfig::Default();
    } else {
        // Directory - look for config.json
        std::string config_path = cartridge_path + "/config.json";
        if (FileExists(config_path)) {
            std::cout << "CartridgeLoader: Loading cartridge with config: " << cartridge_path << std::endl;
            return ParseConfig(config_path);
        } else {
            std::cout << "CartridgeLoader: Loading cartridge without config: " << cartridge_path << std::endl;
            return CartridgeConfig::Default();
        }
    }
}

std::string CartridgeLoader::GetMainLuaPath(const std::string& cartridge_path) {
    if (cartridge_path.find(".lua") != std::string::npos) {
        // It's already a .lua file
        return cartridge_path;
    } else {
        // It's a directory, look for main.lua
        return cartridge_path + "/main.lua";
    }
}

bool CartridgeLoader::IsValidCartridge(const std::string& path) {
    // Check if it's a .lua file
    if (path.find(".lua") != std::string::npos) {
        return FileExists(path);
    }
    
    // Check if it's a directory with main.lua
    if (DirectoryExists(path)) {
        std::string main_lua = path + "/main.lua";
        return FileExists(main_lua);
    }
    
    return false;
}

// === PRIVATE HELPERS ===

bool CartridgeLoader::FileExists(const std::string& path) {
    try {
        return fs::exists(path) && fs::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

bool CartridgeLoader::DirectoryExists(const std::string& path) {
    try {
        return fs::exists(path) && fs::is_directory(path);
    } catch (...) {
        return false;
    }
}

std::string CartridgeLoader::ReadFileContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

std::string CartridgeLoader::ExtractCartridgeName(const std::string& path) {
    fs::path p(path);
    
    // If it's a .lua file, return filename without extension
    if (p.extension() == ".lua") {
        return p.stem().string();
    }
    
    // If it's a directory, return the directory name
    return p.filename().string();
}
