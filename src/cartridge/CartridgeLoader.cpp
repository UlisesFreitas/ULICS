#include "cartridge/CartridgeLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

CartridgeLoader::CartridgeLoader() {
    std::cout << "CartridgeLoader initialized." << std::endl;
}

bool CartridgeLoader::loadCartridge(const std::string& cartridgeDirectoryPath) {
    std::filesystem::path basePath(cartridgeDirectoryPath);
    std::filesystem::path configPath = basePath / "config.json";
    std::filesystem::path scriptPath = basePath / "main.lua";

    // 1. Load and parse config.json
    std::ifstream configFileStream(configPath);
    if (!configFileStream.is_open()) {
        std::cerr << "CartridgeLoader Error: Could not open config file at " << configPath << std::endl;
        return false;
    }

    try {
        configFileStream >> config;
        std::cout << "Cartridge '" << config.value("title", "N/A") << "' config loaded." << std::endl;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "CartridgeLoader Error: Failed to parse " << configPath << ". " << e.what() << std::endl;
        return false;
    }

    // 2. Load main.lua script content
    std::ifstream scriptFileStream(scriptPath);
    if (!scriptFileStream.is_open()) {
        std::cerr << "CartridgeLoader Error: Could not open script file at " << scriptPath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << scriptFileStream.rdbuf();
    luaScript = buffer.str();

    if (luaScript.empty()) {
        std::cerr << "CartridgeLoader Error: Script file at " << scriptPath << " is empty." << std::endl;
        return false;
    }

    loaded = true;
    return true;
}

std::vector<CartridgeInfo> CartridgeLoader::scanForCartridges(const std::string& cartridgesBasePath) {
    std::vector<CartridgeInfo> foundCarts;
    std::filesystem::path basePath(cartridgesBasePath);

    if (!std::filesystem::exists(basePath) || !std::filesystem::is_directory(basePath)) {
        return foundCarts; // Return empty vector if the base path doesn't exist.
    }

    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (entry.is_directory()) {
            std::string dirName = entry.path().filename().string();
            // Skip the hidden boot cartridge and other hidden directories.
            if (dirName.rfind(".", 0) == 0) {
                continue;
            }

            std::filesystem::path configPath = entry.path() / "config.json";
            if (std::filesystem::exists(configPath)) {
                std::ifstream configFileStream(configPath);
                if (configFileStream.is_open()) {
                    try {
                        nlohmann::json config;
                        configFileStream >> config;
                        foundCarts.push_back({
                            dirName, // Use directory name as the ID
                            config.value("title", "Untitled"),
                            config.value("author", "Unknown"),
                            config.value("description", "")
                        });
                    } catch (const nlohmann::json::parse_error&) {
                        // Ignore cartridges with invalid JSON.
                    }
                }
            }
        }
    }
    return foundCarts;
}
