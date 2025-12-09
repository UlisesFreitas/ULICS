#ifndef CARTRIDGE_LOADER_H
#define CARTRIDGE_LOADER_H

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

class CartridgeLoader {
public:
    CartridgeLoader();

    /**
     * @brief Loads a cartridge from the specified directory path.
     * 
     * This function reads the 'config.json' and 'main.lua' files from the
     * given directory, parsing the configuration and loading the script content.
     * @param cartridgeDirectoryPath The path to the cartridge's root folder.
     * @return True if the cartridge was loaded successfully, false otherwise.
     */
    bool loadCartridge(const std::string& cartridgeDirectoryPath);

    /**
     * @brief Scans a directory for valid cartridges.
     * @param cartridgesBasePath The path to the main 'cartridges' folder.
     * @return A vector of CartridgeInfo structs for each valid cartridge found.
     */
    std::vector<CartridgeInfo> scanForCartridges(const std::string& cartridgesBasePath);

    const nlohmann::json& getConfig() const { return config; }
    const std::string& getLuaScript() const { return luaScript; }
    bool isLoaded() const { return loaded; }

private:
    nlohmann::json config;
    std::string luaScript;
    bool loaded = false;
};

#endif // CARTRIDGE_LOADER_H
