#include "cartridge/CartridgeLoader.h"
#include <iostream>

CartridgeLoader::CartridgeLoader() {
    std::cout << "CartridgeLoader initialized." << std::endl;
}

bool CartridgeLoader::loadCartridge(const std::string& path) {
    std::cout << "Attempting to load cartridge from: " << path << std::endl;
    return false; // Placeholder
}
