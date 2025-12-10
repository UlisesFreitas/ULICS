#ifndef CARTRIDGE_LOADER_H
#define CARTRIDGE_LOADER_H

#include "cartridge/Cartridge.h"
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

/// @brief Holds metadata for a single discovered cartridge.
struct CartridgeInfo {
    std::string id;       ///< The directory name, used as a unique identifier.
    std::string title;
    std::string author;
    std::string description;
};

/// @class CartridgeLoader
/// @brief A static utility class for loading cartridge data from disk.
/// This class provides functions to scan for cartridges and load their
/// raw data (config and script) into a Cartridge object. It is stateless.
class CartridgeLoader {
public:
    /**
     * @brief Loads cartridge data from the specified directory path.
     * 
     * This function reads the 'config.json' and 'main.lua' files from the
     * given directory, parsing the configuration and loading the script content.
     * @param cartridgeDirectoryPath The path to the cartridge's root folder.
     * @return A unique_ptr to a Cartridge struct on success, or nullptr on failure.
     */
    static std::unique_ptr<Cartridge> loadCartridge(const std::string& cartridgeDirectoryPath);

    /**
     * @brief Scans a directory for valid cartridges.
     * @param cartridgesBasePath The path to the main 'cartridges' folder.
     * @return A vector of CartridgeInfo structs for each valid cartridge found.
     */
    static std::vector<CartridgeInfo> scanForCartridges(const std::string& cartridgesBasePath);

private:
    // This is now a static utility class, so it has no constructor or members.
    CartridgeLoader() = delete;
};

#endif // CARTRIDGE_LOADER_H
