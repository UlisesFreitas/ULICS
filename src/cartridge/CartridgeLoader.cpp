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
